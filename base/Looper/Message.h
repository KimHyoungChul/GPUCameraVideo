//
// Created by 刘轩 on 2018/12/20.
//

#ifndef GPUCAMERAVIDEO_MESSAGE_H
#define GPUCAMERAVIDEO_MESSAGE_H

#include <string>
#include <thread>
#include "Function.h"

namespace GCVBase {

    /**
     * Message类就是主要的消息结构，该结构中应该有：
     * 1.MessageQueueName 记录当前该Message属于哪个MessageQueue
     * 2.messageFunction 需要进行的各种操作，一般传进来的是Function函数
     *
     * 除了上面的两个基本信息外，Message类还应当维护一个消息缓存池（也用链表实现），用于避免多次重复创建Message对象
     */
    class Message {

    private:
        /*
         * 下面这几个静态的元素都是用于对消息池进行操作的
         */
        static int maxSize;
        static int nowSize;
        static Message * poolHead;

    public:
        std::string mMessageQueueName;
        Function *mMessageFunction;

        Message * next = NULL;

        Message();
        Message(std::string messageQueueName, Function *messageFunction);
        ~Message();

        void setMessageFunction(Function *mMessageFunction);
        void setMessageQueueName(std::string messageQueueName);
        std::string getMessageQueueName();


        static void setMaxPoolSize(int maxSize);
        static int getNowPoolSize();
        static bool isNowMaxPool(); //现在缓存池是最大容量吗？
        static Message * obtain();
        static void recycle(Message * message);

        static void clearMessagePool(); //用于在项目销毁时释放Message缓存池中的Message对象
        static void resetMessage(Message * message); //重置Message中的信息
    };
}


#endif //GPUCAMERAVIDEO_MESSAGE_H
