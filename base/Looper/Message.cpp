//
// Created by 刘轩 on 2018/12/20.
//

#include "Message.h"

int GCVBase::Message::maxSize = 20;
int GCVBase::Message::nowSize = 0;
GCVBase::Message * GCVBase::Message::poolHead = NULL;


/**
 * 所有的方法都在Looper中统一做加锁等多线程相关的处理
 */
GCVBase::Message *GCVBase::Message::obtain() {
    Message *m = poolHead;

    if (poolHead != NULL) {
        poolHead = m->next;
        m->next = NULL;
        nowSize--;
        return m;
    } else {
        m = new Message();
    }
    return m;
}

void GCVBase::Message::recycle(GCVBase::Message *message) {

    resetMessage(message); //回收时先重置Message对象中的各种信息

    if(poolHead != NULL){
        if(!isNowMaxPool()) {
            Message *temp = poolHead;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = message;
        } else{
            // TODO 这里加一些删除操作的提示信息，用于判断缓存池容量大小是否合适，也可以在这里做一些逻辑用于决定缓存池是否扩容
            delete(message); //如果此时缓存池已经是最大容量了，就不要再缓存了，直接删掉
        }
    } else{
        // TODO 检查 nowSize 的值，如果不为0那就有问题了
        poolHead = message;
    }
    nowSize++;
}

void GCVBase::Message::resetMessage(Message * message) {
    message->next = NULL;
    message->setMessageQueueName(NULL); //reset的时候主要把next和QueueName置空，Function对象不要动，可以重复利用
}

void GCVBase::Message::clearMessagePool() {
    while(poolHead != NULL){ //清空缓存池中的缓存消息
        Message *m = poolHead;
        poolHead = m->next;
        m->next = NULL;
        delete(m);
    }
}

GCVBase::Message::Message() {
    Message(NULL, NULL);
}

GCVBase::Message::Message(std::string messageQueueName, Function *messageFunction) {
    mMessageQueueName = messageQueueName;
    mMessageFunction = messageFunction;
}


GCVBase::Message::~Message() {
    delete(mMessageFunction);
}


void GCVBase::Message::setMessageQueueName(std::string messageQueueName) {
    mMessageQueueName = messageQueueName;
}

std::string GCVBase::Message::getMessageQueueName() {
    return mMessageQueueName;
}

void GCVBase::Message::setMessageFunction(GCVBase::Function *messageFunction) {
    mMessageFunction = messageFunction;
}

void GCVBase::Message::setMaxPoolSize(int max) {
    maxSize = max;
}

int GCVBase::Message::getNowPoolSize() {
    return nowSize;
}

bool GCVBase::Message::isNowMaxPool() {
    return nowSize >= maxSize;
}














