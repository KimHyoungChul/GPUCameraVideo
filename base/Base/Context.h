//
// Created by 刘轩 on 2018/12/24.
//

#ifndef GPUCAMERAVIDEO_CONTEXT_H
#define GPUCAMERAVIDEO_CONTEXT_H

#include <Looper.h>
#include <EglCore.h>
#include <jni.h>

namespace GCVBase {

    class Context {

    private:
        JNIEnv *mContextEnv = NULL;
        JavaVM *mContextJavaVM = NULL;

        EglCore * mEglInstance = NULL;
        Looper * mContextLooper = NULL;

        static Context * mShareContext;

    public:
        Context(JNIEnv *env, std::string looperName);
        ~Context();

        Looper *getContextLooper();
        void setNativeWindow(ANativeWindow * window);
        void setEglInstance(EglCore * eglInstance);
        EglCore * getEglInstance();
        void useAsCurrentContext();

        JNIEnv * getEnv() const;

        static void makeShareContextAsCurrent();

        static void initSharedContext(JNIEnv * env);
        static Context * getShareContext();
    };

    extern "C"{
        void runSyncContextLooper(Looper * mLooper, const std::function<void()> &function);
        void runAsyncContextLooper(Looper * mLooper, const std::function<void()> &function);
    }
}


#endif //GPUCAMERAVIDEO_CONTEXT_H
