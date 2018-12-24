//
// Created by 刘轩 on 2018/12/22.
//

#include "MessageQueue.h"

GCVBase::MessageQueue::MessageQueue(std::string looperName, std::string messageQueueName) {

    // TODO 在这里加断言，这两个东西都必须有值的

    mLooperName = looperName;
    mMessageQueueName = messageQueueName;
}

void GCVBase::MessageQueue::addMessage(GCVBase::Message *message) {

    // TODO 这里应该加入message有效性的校验，比如说非空判断什么的

    message->setMessageQueueName(mMessageQueueName); //给message设置所在队列的名称，方便回溯

    if(queueHead != NULL){
        Message * temp = queueHead;
        while (temp->next != NULL){
            temp = temp->next;
        }
        temp->next = message;
    } else{
        queueHead = message;
    }
}

GCVBase::Message *GCVBase::MessageQueue::nextMessage() {

    if (queueHead == NULL) { //等于NULL说明此时没有消息了
        return NULL;
    }

    Message *m = queueHead;
    queueHead = m->next;
    m->next = NULL;

    return m;
}

void GCVBase::MessageQueue::recycleMessage(GCVBase::Message *message) {
    Message::recycle(message);
}

void GCVBase::MessageQueue::recycleAllMessage() {
    while(queueHead != NULL){
        Message *m = queueHead;
        queueHead = m->next;
        m->next = NULL;
        //TODO 加一个提示，就说哪个哪个Message被回收了
        Message::recycle(m);
    }
}

void GCVBase::MessageQueue::setMessageQueueName(std::string messageQueueName) {
    mMessageQueueName = messageQueueName;
}

std::string GCVBase::MessageQueue::getMessageQueueName() {
    return mMessageQueueName;
}



GCVBase::MessageQueue::~MessageQueue() {
    recycleAllMessage(); //此时消息队列中的消息还是没执行完，就不等了,直接回收
}




