//
// Created by 刘轩 on 2018/12/27.
//

#include <android/log.h>
#include <Size.hpp>
#include <FilterGroup.h>
#include "Camera.h"

std::string cameraVertexShader =
                "attribute vec4 aPosition;\n"
                "attribute vec4 aTexCoord;\n"
                "varying vec2 vTexCoord;\n"
                "void main() {\n"
                "   gl_Position = aPosition;\n"
                "   vTexCoord = aTexCoord.xy;\n"
                "}\n";

std::string cameraFragmentShader =
                "#extension GL_OES_EGL_image_external : require\n"
                "precision mediump float;\n"
                "varying vec2 vTexCoord;\n"
                "uniform samplerExternalOES uTexture;\n"
                "void main() {\n"
                "    gl_FragColor = texture2D(uTexture, vTexCoord);\n"
                "}\n";


GCVBase::Camera::Camera(int mFacing) {

    mFacingMode = mFacing == 0 ? FacingMode::FacingBack : FacingMode::FacingFront;
    rotationCamera = Rotation(rotationMode, mFacingMode);

    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [=] {
        Context::makeShareContextAsCurrent();

        __android_log_print(ANDROID_LOG_ERROR, "Camera", "Camera Thread is %u", std::this_thread::get_id());

        mCameraProgram = new GLProgram(cameraVertexShader, cameraFragmentShader);

        if(!mCameraProgram->isProgramLinked()){


            if(!mCameraProgram->linkProgram()){
                // TODO 获取program连接日志`
            }
        }

        aPositionAttribute = mCameraProgram->getAttributeIndex("aPosition");
        aTexCoordAttribute = mCameraProgram->getAttributeIndex("aTexCoord");
        uTextureuniform = mCameraProgram->getuniformIndex("uTexture");
    });

}

GCVBase::Camera::~Camera() {

}

std::string GCVBase::Camera::VertexShared() {
    return cameraVertexShader;
}

std::string GCVBase::Camera::FragmentShared() {
    return cameraFragmentShader;
}

GCVBase::EglCore *GCVBase::Camera::getEglInstance() {
    return mEglInstance;
}

/**
 * 这个函数是在Java层的 SurfaceHolder.Callback 线程中的，而我们的 sharedEglInstance 中的 eglInstance是在他自己的
 * Looper 对应的线程中的，因此这里不能调sharedEglInstance（主Context）的eglMakeCurren函数，会出事情
 *
 * 同时，这个函数中初始化了OES扩展纹理，这个纹理就是SurfaceTexture中绑定的纹理，后续系统的渲染函数都会到这个纹理上来，但是
 * 同样因为这个纹理是在 SurfaceHolder.Callback 线程中，因此在 sharedEglInstance 对应的线程中是不能对这个纹理进行操作的
 *
 * 因此我们这里用到了Egl的ShareContext（共享上下文）,也就是重新new 一个 EglCore 对象，将上面的 sharedEglInstance 作为
 * 共享上下文传入 eglCreateContext 函数中，这样的话两个线程就可以同时对 mOESTexture 进行操作了
 *
 * 这样的话我们就可以在下面的 surfaceTextureAvailable 调用 sharedEglInstance的 eglSwapBuffers 函数，从而将渲染结果
 * 交换到前台，显示到屏幕上了
 *
 * 同时需要注意的是，我们新new 的 EglCore 对象中第二个参数（ANativeWindow * ）是NULL，从对应的源码中我们可以看到，在这个
 * 逻辑中我们创建EGLSurface 的是eglCreatePbufferSurface()，这个函数只是在后台创建一个数据缓冲区，用于缓存这一帧的像素
 * 数据，之所以这么做是因为我们考虑后面在录制视频的时候要加一个写入线程，将缓冲区中的数据编码、写入本地，这样预览就不会卡了
 */
void GCVBase::Camera::genSurfaceTexture() {

        __android_log_print(ANDROID_LOG_ERROR, "genSurfaceTexture", "genSurfaceTexture Thread is %u", std::this_thread::get_id());

        if (!mEglInstance) {
            const EglCore *sharedEglInstance = (const EglCore *)Context::getShareContext()->getEglInstance();
            mEglInstance = new EglCore( (const void *)sharedEglInstance->getEGLContext(), NULL, 1, 1);
            mEglInstance->makeAsCurrent();

            glGenTextures(1, &mOESTexture);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, mOESTexture);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
        }
}

GLuint GCVBase::Camera::getSurfaceTexture() {
    return mOESTexture;
}

void GCVBase::Camera::onSurfaceChanged() {

    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [=] {
        if(mOutputFrameBuffer){
            delete mOutputFrameBuffer;
        }
//        Size framebufferSize = rotationMode == RotationMode::rotation0 ? Size(mPreviewWidth, mPreviewHeight) : Size(mPreviewHeight, mPreviewWidth);
        Size framebufferSize = Size(mPreviewWidth, mPreviewHeight);
        mOutputFrameBuffer = new FrameBuffer(framebufferSize, mOutputTextureOptions, Context::getShareContext());
    });
}

void GCVBase::Camera::surfaceTextureAvailable() {
    glFlush();

    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [=]{
        Context::makeShareContextAsCurrent();
        mCameraProgram->useProgram();

        glEnableVertexAttribArray(aPositionAttribute);
        glEnableVertexAttribArray(aTexCoordAttribute);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, mOESTexture);

        static const GLfloat vertices[] = {
                -1.0f, -1.0f,
                1.0f, -1.0f,
                -1.0f,  1.0f,
                1.0f,  1.0f,
        };

        static const GLfloat texCoord[] = { //这里对纹理坐标不进行任何处理，在输出部分（DisplayView、MediaRecorder处做最终的处理）
                0.0f, 0.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
        };

        glUniform1i(uTextureuniform, 0);
        glVertexAttribPointer(aPositionAttribute, 2, GL_FLOAT, 0, 0, vertices);
        glVertexAttribPointer(aTexCoordAttribute, 2, GL_FLOAT, 0, 0, texCoord);

        mOutputFrameBuffer->bindFramebuffer();

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); //只需要在该步骤之前绑定帧缓冲即可，该步骤所有的绘制都会渲染到帧缓冲中的纹理上

        mOutputFrameBuffer->unbindFramebuffer(); // DisplayView绘制之前必须解绑帧缓冲对象，否则系统自带的帧缓冲（0）渲染不出来！！！！！！！

        glDisableVertexAttribArray(aPositionAttribute);
        glDisableVertexAttribArray(aTexCoordAttribute);

        MediaTime time = MediaTime((int64_t)currentTimeOfMilliseconds(), 60, TimeFlag_Init);
        newFrameReadyAtTime(time);
    });
}

void GCVBase::Camera::newFrameReadyAtTime(const MediaTime &time) {
    for(auto i = mTargets.begin(); i < mTargets.end(); i++){
        auto currentTarget =  * i;
        FilterGroup * filterGroup = (FilterGroup *) (jlong)currentTarget;
        filterGroup->_setOutputRotation(rotationCamera);
        filterGroup->_setOutputFramebuffer(mOutputFrameBuffer);
        filterGroup->_newFrameReadyAtTime(time);
    }
}

void GCVBase::Camera::orientationChanged(int orientation) {
    switch (orientation){
        case 0:
            rotationMode = RotationMode::rotation0;
            break;
        case 90:
            rotationMode = RotationMode::rotation90;
            break;
        case 270:
            rotationMode = RotationMode::rotation270;
            break;
        default:
            break;
    }
    rotationCamera = Rotation(rotationMode, mFacingMode);
}

void GCVBase::Camera::facingChanged(int facing) {
    mFacingMode = facing == 0 ? FacingMode::FacingBack : FacingMode::FacingFront;
    rotationCamera = Rotation(rotationMode, mFacingMode);
}

