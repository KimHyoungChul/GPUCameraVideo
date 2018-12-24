//
// Created by 刘轩 on 2018/12/20.
//

#include "Looper.h"

void GCVBase::Looper::runFunctionInLock(const std::function<void()> &function) {
    mFunctionLock.lock();
    function();
    mFunctionLock.unlock();
}

GCVBase::Looper::Looper(const std::string &name) {
    mLooperName = name;

    mMessageQueue = new MessageQueue(mLooperName, mLooperName);
    queueConditionLock = new ConditionLock([&]() -> bool{
        return mMessageQueue->nextMessage() != NULL || mQuit;
    });

    mWorkThread = new std::thread(&Looper::loop, this);
    threadId = mWorkThread->get_id();
}

GCVBase::Looper::~Looper() {
    mQuit = true;

    queueConditionLock ->notifyOne(); //有可能此时Looper线程还在wait，唤醒之，这句只能在 mQuit = true; 之后
    mWorkThread->join(); //让当前运行线程等待Looper线程执行完毕，再销毁

//    Message::clearMessagePool();

    delete(mWorkThread);
    delete(mMessageQueue);
    delete(queueConditionLock);
}

std::string GCVBase::Looper::getLooperName() {
    return mLooperName;
}

/**
 * 首先要明确，这个addMessage是在别的线程中添加的，相当于生产者
 * @param function
 */
void GCVBase::Looper::addMessageAsync(const std::function<void()> &function) {
    addMessage(function, true);
}

void GCVBase::Looper::addMessageSync(const std::function<void()> &function) {
    addMessage(function, false);
}

void GCVBase::Looper::addMessage(const std::function<void()> &function, bool async) {
    if(mQuit){
        return;
    }

    Function * fun = NULL;
    Message * message = NULL;
     //这个方法本来就是同步方法
    runFunctionInLock([&]{
        message = Message::obtain();
        if(message->mMessageFunction) {
            fun = new Function(function, async);
            message->mMessageFunction = fun;
        } else{
            message->mMessageFunction->setFunction(function, async);
            fun = message->mMessageFunction;
        }
        message->mMessageQueueName = mMessageQueue->getMessageQueueName();
        mMessageQueue->addMessage(message);
        queueConditionLock->notifyOne(); //加入MessageQueue之后，唤醒等待的Looper线程，loop()方法就开始循环
    });

    if(!async){
        /**
         * 同步意味着必须等这个Function执行完毕，当前线程才能继续运行。
         *   注意此时是对functionConditionLock加的锁，所以addMessage线程会被阻塞在这个condition上，
         * looper线程已经被queueConditionLock唤醒了，所以正常执行。
         *   而且这个functionConditionLock是每一个Function对象独有的，所以必须等待该fun对象执行完后，
         * 这边才能被唤醒，然后执行fun->forceWaitFunctionFinish();后面的语句
         */
        fun->forceWaitFunctionFinish();
        runFunctionInLock([&]{
            mMessageQueue->recycleMessage(message); //执行完毕后回收同步消息
        });
    }
}

/**
 * loop()这个方法时在Looper对应的线程中运行，相当于消费者。
 *
 *    我们规定一个Looper对象对应一个线程，且在Looper的构造函数中创建了条件锁对象，也就是说一个Looper对象拥有一个
 * queueConditionLock对象，该condition锁只能用来阻塞当前的Looper线程和addMessage线程
 *    因此，不管你new几个Looper，其实这里通过queueConditionLock条件等待的只有两个线程：Looper（消费者）线程和
 * addMessage（生产者）线程，其中消费的消息队列也只是他们两个人的。
 */
void GCVBase::Looper::loop() {

    while(!mQuit) {

        Message * message = NULL;
        Function * function = NULL;
        runFunctionInLock([&]{
            message = mMessageQueue->nextMessage();
            if(message != NULL){
                function = message->mMessageFunction;
            }
        });

        if(function){
            bool isAsync = function->isAsync();
            function->run(); //run完了就finish了
            function->notifyOne(); //唤醒functionConditionLock上等待的addMessage线程,继续执行fun->forceWaitFunctionFinish()之后的代码
            runFunctionInLock([&]{
                if(isAsync){ //这里是回收异步线程的message,同步消息上面已经回收过了
                    mMessageQueue->recycleMessage(message);
                }
            });
        }
        queueConditionLock->verificationCondition(); //如果这个Queue的消息队列为空，阻塞本线程，不然这个while就没完没了了
    }

    runFunctionInLock([=]{ //循环停掉后MessageQueue可能还有消息，应当全部回收
        mMessageQueue->recycleAllMessage();
    });
}



