//
// Created by 刘轩 on 2018/12/20.
//

#ifndef GPUCAMERAVIDEO_FUNCTION_H
#define GPUCAMERAVIDEO_FUNCTION_H

#include "ConditionLock.hpp"

namespace GCVBase {

    class Function {

    private:
        std::function<void()> mFunction = NULL;
        ConditionLock * functionConditionLock = NULL;

        bool mAsync = false;
        bool mFinished = false;

    public:

        Function(const std::function<void()> &function, bool async){
            mAsync = async;
            mFunction = function;

            functionConditionLock = new ConditionLock([&]()-> bool{
                return mFinished;
            });
        }

        ~Function(){
            delete functionConditionLock;
        }

        void forceWaitFunctionFinish(){
            functionConditionLock->verificationCondition();
        }

        void run(){
            if(mFunction){
                mFunction();
            }
            mFinished = true;
        }

        void notifyOne(){
            functionConditionLock->notifyOne();
        }

        void setFunction(const std::function<void()> &function, bool async){
            mFunction = function;
            mAsync = async;
            mFinished = false;
        }

        bool isAsync(){
            return mAsync;
        }
    };
}


#endif //GPUCAMERAVIDEO_FUNCTION_H
