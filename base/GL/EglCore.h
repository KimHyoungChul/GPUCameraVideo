//
// Created by 刘轩 on 2018/12/25.
//

#ifndef GPUCAMERAVIDEO_EGLCORE_H
#define GPUCAMERAVIDEO_EGLCORE_H

#include <string>
#include <EGL/egl.h>
#include <android/native_window.h>
#include <GLES2/gl2.h>

namespace GCVBase {

    class EglCore {

    private:
        EGLContext mEGLContext = 0;
        EGLDisplay mEGLDisplay = 0;
        EGLSurface mEGLSurface = 0;
        EGLConfig  mEGLConfig = 0;

        ANativeWindow * mWindow = 0;

        const void *mSharedObject = NULL;

        void checkEglError(std::string msg);

    public:
        EglCore(const void * sharedContext = NULL, ANativeWindow * window = NULL, int width = 0, int height = 0);
        ~EglCore();

        EGLContext getEGLContext() const;

        bool isCurrentContext();
        void makeAsCurrent();
        void swapToScreen();
        void resetSurface(ANativeWindow * window);

        const void * getSharedObject();
    };
}


#endif //GPUCAMERAVIDEO_EGLCORE_H
