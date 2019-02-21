//
// Created by 刘轩 on 2018/12/23.
//

#ifndef GPUCAMERAVIDEO_CONDITIONLOCK_H
#define GPUCAMERAVIDEO_CONDITIONLOCK_H

#include <mutex>
#include <condition_variable>

class ConditionLock {

private:
    std::mutex mutex;
    std::condition_variable mConditionVariable;
    /*
     * 验证条件，不同的Condition锁有不同的完成条件：
     * 对于queueConditionLock，条件就是queue中的message执行完毕或者退出
     * 对于functionConditionLock，条件就是该Function执行完毕
     */
    std::function<bool()> mVerificationCondition;

public:
    ConditionLock(const std::function<bool()> &condition){
        mVerificationCondition = condition;
    }

    void verificationCondition(){
        if(!mVerificationCondition){
            return;
        }
        std::unique_lock<decltype(mutex)> lock(mutex);
        while (!mVerificationCondition()){
            mConditionVariable.wait(lock);
        }
    }

    void notifyOne(){
        std::unique_lock<decltype(mutex)> lock(mutex);
        mConditionVariable.notify_one();
    }

    void notifyAll(){
        std::unique_lock<decltype(mutex)> lock(mutex);
        mConditionVariable.notify_all();
    }
};

#endif //GPUCAMERAVIDEO_CONDITIONLOCK_H
