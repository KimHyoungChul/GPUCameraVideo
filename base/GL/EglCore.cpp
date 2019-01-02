//
// Created by 刘轩 on 2018/12/25.
//

#include <thread>
#include "EglCore.h"

GCVBase::EglCore::EglCore(const void *sharedContext, ANativeWindow *window, int width, int height) {

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    checkEglError("eglCreateEglDisplay");

    if (display == EGL_NO_DISPLAY) {
        throw "eglGetDisplay failed";
    }

    EGLint maxVersion, minVersion;
    EGLBoolean initResult = eglInitialize(display, &maxVersion, &minVersion);

    EGLConfig config = NULL;
    EGLint numConfigs = 0;

    EGLint confAttr[15] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE,   8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE,  8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };

    if (!eglChooseConfig(display, confAttr, &config, 1, &numConfigs) || numConfigs != 1) {
        throw "eglChooseConfig failed";
    }

    EGLint ctxAttr[3] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

    if(window){ // 创建可显示的surface

        int surfaceAttribs[] = {EGL_NONE};
        EGLSurface surface = eglCreateWindowSurface(display, config, window, surfaceAttribs);
        checkEglError("eglCreateWindowSurface");

        EGLContext context;
        if(sharedContext) {
            EGLContext sharedGLContext = (EGLContext) sharedContext;
            context = eglCreateContext(display, config, sharedGLContext, ctxAttr);
        } else{
            context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
        }

        if (context == EGL_NO_CONTEXT) {
            throw "eglCreateContext failed";
        }

        mWindow = window;
        mEGLSurface = surface;
        mEGLContext = context;
        mEGLDisplay = display;
        mEGLConfig = config;

    } else{ //创建后台像素缓冲区

        int surfaceAttribs[] = {EGL_WIDTH, width, EGL_HEIGHT, height, EGL_NONE};

        EGLSurface surface = eglCreatePbufferSurface(display, config, surfaceAttribs);
        checkEglError("eglCreatePbufferSurface");

        EGLContext sharedGLContext = (EGLContext)sharedContext;
        EGLContext context = eglCreateContext(display, config, sharedGLContext, ctxAttr);

        if (context == EGL_NO_CONTEXT) {
            throw "eglCreateContext failed";
        }

        mEGLSurface = surface;
        mEGLContext = context;
        mEGLDisplay = display;
        mEGLConfig = config;
    }

    mSharedObject = sharedContext;
}

GCVBase::EglCore::~EglCore() {
    eglDestroySurface(mEGLDisplay, mEGLSurface);
    eglDestroyContext(mEGLDisplay, mEGLContext);
    eglTerminate(mEGLDisplay);
    eglReleaseThread();

    if (mWindow) {
        ANativeWindow_release(mWindow);
    }
}

EGLContext GCVBase::EglCore::getEGLContext() const {
    return mEGLContext;
}

void GCVBase::EglCore::checkEglError(std::string msg) {
    int error;
    if ((error = eglGetError()) != EGL_SUCCESS) {
        throw msg + ": EGL error: 0x" + std::to_string(error);
    }
}

bool GCVBase::EglCore::isCurrentContext() {

    return eglGetCurrentContext() == mEGLContext;
}

/**
 * eglMakeCurrent函数（包括他的参数）是线程相关的，如果你在A线程中创建了mEGLDisplay、mEGLContext、mEGLSurface
 * 这些对象，却在B线程中调用了 eglMakeCurrent 方法，此时就会爆出 eglMakeCurrent:866 error 3002 (EGL_BAD_ACCESS)错误
 *
 * 如果 eglMakeCurrent函数没有得到有效调用，那么opengl的各个函数都不会生效 ！！！
 */
void GCVBase::EglCore::makeAsCurrent() {
    eglMakeCurrent(mEGLDisplay, mEGLSurface, mEGLSurface , mEGLContext);
}

/**
 * 如果一切都进展的顺利的话，在调用该函数后，我们就能在对应的屏幕上看到画面了
 */
void GCVBase::EglCore::swapToScreen() {
    eglSwapBuffers(mEGLDisplay, mEGLSurface);
}

void GCVBase::EglCore::resetSurface(ANativeWindow *window) {
    if(!window){
        return;
    }

    if(mEGLSurface){
        if(!isCurrentContext()){
            makeAsCurrent();
        }

        int surfaceAttribs[] = {EGL_NONE};
        mEGLSurface = eglCreateWindowSurface(mEGLDisplay, mEGLConfig, window, surfaceAttribs);

        makeAsCurrent();
    }
}

const void * GCVBase::EglCore::getSharedObject() {
    return mSharedObject;
}
