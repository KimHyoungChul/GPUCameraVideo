//
// Created by 刘轩 on 2018/12/24.
//

#include "Context.h"

GCVBase::Context * GCVBase::Context::mShareContext = nullptr;

GCVBase::Context::Context(JNIEnv *env, std::string looperName) {
    mContextLooper = new Looper(looperName);

    env->GetJavaVM(&mContextJavaVM);
    mContextLooper->addMessageSync([=]{
        int result = mContextJavaVM->GetEnv((void **)&mContextEnv, JNI_VERSION_1_6);
        if (result == JNI_EDETACHED) {

            /* JNIEnv指针仅在创建它的线程有效。如果我们需要在其他线程访问JVM，那么必须先调用AttachCurrentThread将
             * 当前线程与JVM进行关联，然后才能获得JNIEnv对象。当然，我们在必要时需要调用DetachCurrentThread来解除链接。
             */
            if (mContextJavaVM->AttachCurrentThread(&mContextEnv, nullptr) != 0) {
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
    // 这里要做线程转换，因为 setNativeWindow 函数是在 SurfaceHolder.Callback 线程中的
    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [&]{
        mEglInstance = new EglCore(NULL, window, 0, 0); //这里我们是设置Native层的window,shareContext直接置空就可以了
    });

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
        mShareContext =  new Context(env, "mainLooper");
    });
}

GCVBase::Context *GCVBase::Context::getShareContext() {
    return mShareContext;
}

GCVBase::Looper *GCVBase::Context::getContextLooper() {
    return mContextLooper;
}

void GCVBase::Context::makeShareContextAsCurrent() {
    Context * shareContext = Context::getShareContext();
    EglCore * shareEglContext = shareContext->getEglInstance();
    if(!shareEglContext->isCurrentContext()){
        shareEglContext->makeAsCurrent();
    }
}

void GCVBase::Context::useAsCurrentContext() {
    EglCore * mEglContext = getEglInstance();
    if(!mEglContext->isCurrentContext()){
        mEglContext->makeAsCurrent();
    }
}

/**
 * 一个Context对应一个Looper，一个Looper对应一个线程，因此这个函数的作用是往不同Looper对应线程的MessageQueue中添加任务
 *
 * @param mLooper 需要将任务添加到哪个Looper中，这个Looper可以使Context中的Looper，也可以是单独new出来的Looper
 * @param function 需要执行的任务，一般传进来的多是Lamda表达式
 */
void GCVBase::runSyncContextLooper(Looper * mLooper, const std::function<void()> &function) {
    if(mLooper->isCurrentThread()){
        function();
    } else{
        mLooper->addMessageSync(function);
    }
}

void GCVBase::runAsyncContextLooper(Looper * mLooper, const std::function<void()> &function) {
    if(mLooper->isCurrentThread()){
        function();
    } else{
        mLooper->addMessageAsync(function);
    }
}
