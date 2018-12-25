//
// Created by 刘轩 on 2018/12/24.
//

#include "Context.h"

GCVBase::Context * GCVBase::Context::mShareContext = NULL;

GCVBase::Context::Context(JNIEnv *env, std::string looperName) {
    mContextLooper = new Looper(looperName);

    env->GetJavaVM(&mContextJavaVM);
    mContextLooper->addMessageSync([=]{
        int result = mContextJavaVM->GetEnv((void **)&mContextEnv, JNI_VERSION_1_6);
        if (result == JNI_EDETACHED) {

            /* JNIEnv指针仅在创建它的线程有效。如果我们需要在其他线程访问JVM，那么必须先调用AttachCurrentThread将
             * 当前线程与JVM进行关联，然后才能获得JNIEnv对象。当然，我们在必要时需要调用DetachCurrentThread来解除链接。
             */
            if (mContextJavaVM->AttachCurrentThread(&mContextEnv, NULL) != 0) {
                // TODO 报错 "Failed to attach";
            }
        } else if (result == JNI_OK) {
            // TODO 提示成功
        } else if (result == JNI_EVERSION) {
            // TODO 报错 "GetEnv: version not supported";
        }
    });
}

GCVBase::Context::~Context() {
    if(mContextLooper->isCurrentThread()){
        delete mEglInstance;
    } else{
        mContextLooper->addMessageAsync([=]{
            delete mEglInstance;
        });
    }

    delete mContextLooper;
}


void GCVBase::Context::setNativeWindow(ANativeWindow *window) {
    if(!window){
        //TODO 报错
    }
    mEglInstance = new EglCore(NULL, window, 0, 0); //这里我们是设置Native层的window,shareContext直接置空就可以了

}

void GCVBase::Context::setEglInstance(GCVBase::EglCore *eglInstance) {
    if(!eglInstance){
        // TODO 报错
    }
    mEglInstance = eglInstance;
}

GCVBase::EglCore *GCVBase::Context::getEglInstance() {
    return mEglInstance;
}

JNIEnv *GCVBase::Context::getEnv() const {
    if(!mContextLooper->isCurrentThread()){
        // TODO 报错并提示
    }
    return mContextEnv;
}

void GCVBase::Context::initSharedContext(JNIEnv *env) {
    static std::once_flag flag;
    std::call_once(flag, [=]{
        mShareContext =  new Context(env, "shareLoop");
    });
}

GCVBase::Context *GCVBase::Context::getShareContext() {
    return mShareContext;
}

GCVBase::Looper *GCVBase::Context::getContextLooper() {
    return mContextLooper;
}


void GCVBase::runSyncContextQueue(Context *context, const std::function<void()> &function) {
    if(context->getContextLooper()->isCurrentThread()){
        function();
    } else{
        context->getContextLooper()->addMessageSync(function);
    }
}

void GCVBase::runAsyncContextQueue(Context *context, const std::function<void()> &function) {
    if(context->getContextLooper()->isCurrentThread()){
        function();
    } else{
        context->getContextLooper()->addMessageAsync(function);
    }
}
