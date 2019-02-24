//
// Created by 刘轩 on 2018/12/22.
//

#include "MessageQueue.h"
#include <utility>

GCVBase::MessageQueue::MessageQueue(std::string looperName, std::string messageQueueName) {

    // TODO 在这里加断言，这两个东西都必须有值的

    mLooperName = std::move(looperName);
    mMessageQueueName = std::move(messageQueueName);
}

void GCVBase::MessageQueue::addMessage(GCVBase::Message *message) {

    // TODO 这里应该加入message有效性的校验，比如说非空判断什么的

    message->setMessageQueueName(mMessageQueueName); //给message设置所在队列的名称，方便回溯

    if(queueHead != nullptr){
        Message * temp = queueHead;
        while (temp->next != nullptr){
            temp = temp->next;
        }
        temp->next = message;
    } else{
        queueHead = message;
    }
}

GCVBase::Message *GCVBase::MessageQueue::getNextMessage() {

    if (queueHead == nullptr) { //等于NULL说明此时没有消息了
        return nullptr;
    }

    Message *m = queueHead;
    queueHead = m->next;
    m->next = nullptr;

    return m;
}



void GCVBase::MessageQueue::recycleMessage(GCVBase::Message *message) {
    Message::recycle(message);
}

void GCVBase::MessageQueue::recycleAllMessage() {
    while(queueHead != nullptr){
        Message *m = queueHead;
        queueHead = m->next;
        m->next = nullptr;
        //TODO 加一个提示，就说哪个哪个Message被回收了
        Message::recycle(m);
    }
}

void GCVBase::MessageQueue::setMessageQueueName(std::string messageQueueName) {
    mMessageQueueName = std::move(messageQueueName);
}

std::string GCVBase::MessageQueue::getMessageQueueName() {
    return mMessageQueueName;
}



GCVBase::MessageQueue::~MessageQueue() {
    recycleAllMessage(); //此时消息队列中的消息还是没执行完，就不等了,直接回收
}

bool GCVBase::MessageQueue::isMessageQueueEmpty() {
    return queueHead == nullptr;
}





