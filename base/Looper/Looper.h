//
// Created by 刘轩 on 2018/12/20.
//

#ifndef GPUCAMERAVIDEO_LOOPER_H
#define GPUCAMERAVIDEO_LOOPER_H

#include "MessageQueue.h"
#include "Function.h"

namespace GCVBase {

    class Looper {

    private:

        std::string mLooperName;
        std::thread::id threadId;

        std::mutex mFunctionLock;
        std::thread * mWorkThread = NULL;
        ConditionLock *queueConditionLock = NULL;

        MessageQueue * mMessageQueue;

        bool mQuit = false;
        void loop();
        void runFunctionInLock(const std::function<void()> &function);

    public:
        Looper(const std::string &name);
        ~Looper();

        std::string getLooperName();

        void addMessageAsync(const std::function<void()> &function);
        void addMessageSync(const std::function<void()> &function);
        void addMessage(const std::function<void()> &function, bool async);

        bool isCurrentThread(){
            return std::this_thread::get_id() == mWorkThread->get_id();
        }
    };
}


#endif //GPUCAMERAVIDEO_LOOPER_H
