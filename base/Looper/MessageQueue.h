//
// Created by 刘轩 on 2018/12/22.
//

#ifndef GPUCAMERAVIDEO_MESSAGEQUEUE_H
#define GPUCAMERAVIDEO_MESSAGEQUEUE_H

#include "Message.h"

namespace GCVBase {

    /**
     * 每一个线程持有一个Looper,每个Looper持有一个MessageQueue对象，
     * Message由MessageQueue管理，而MessageQueue由Looper管理，因此Looper不直接持有Message的引用
     *
     * MessageQueue类主要用于维护一个Message链表组成的消息队列，包括出队、入队、循环遍历等操作：
     * 1.mLooperName 主要标识该MessageQueue所在的Looper名称
     * 2.mMessageQueueName 主要用于标识messageQueue中的message
     * 3.queueHead 指针用于表示这个MessageQueue中Message链表的头结点
     */
    class MessageQueue {

    private:

        std::string mLooperName;
        std::string mMessageQueueName;

        Message * queueHead = NULL;

    public:

        MessageQueue(std::string looperName, std::string messageQueueName); //这两个参数是必须指定的
        ~MessageQueue();

        void setMessageQueueName(std::string messageQueueName);
        std::string getMessageQueueName();

        void addMessage(Message * message);
        bool isMessageQueueEmpty();
        Message * getNextMessage(); //获得消息队列中下一个Message对象，并从链表中删除
        void recycleMessage(Message * message); //用于处理完Message中的内容后回收并缓存Message对象
        void recycleAllMessage();
    };
}


#endif //GPUCAMERAVIDEO_MESSAGEQUEUE_H
