//
// Created by 刘轩 on 2019/1/10.
//

#ifndef GPUCAMERAVIDEO_MEDIARECORD_H
#define GPUCAMERAVIDEO_MEDIARECORD_H

#include <jni.h>
#include "Context.h"
#include "GLProgram.h"
#include "NativeOutput.h"
#include "Encode/MediaEncoder.hpp"

namespace GCVBase {

    class MediaRecorder : NativeOutput{

    private:

        GLuint aPositionAttribute = 0;
        GLuint aTexCoordAttribute = 0;
        GLuint uTextureuniform = 0;

        Size mVideoSize = Size::zero();
        MediaTime mStartTime = MediaTime::Init();
        MediaTime mPreviousFrameTime = MediaTime::NegativeInfinity();
        MediaTime mPreviousAudioTime = MediaTime::NegativeInfinity();

        GLubyte * mRGBAData = NULL;
        GLuint mRecorderTexture = 0;
        GLuint mRecorderFramebuffer = 0;

        MediaEncoder * mediaEncoder = NULL;
        Context * recordContext = NULL;
        GLProgram *mRecordProgram = NULL;

        bool mIsRecording = false;
        bool mCancelRecording = false;
        bool mEncoderIsFinished = false;

        MediaBuffer<GLubyte *> *mediaBuffer  = NULL;

        Rotation rotationMediaRecorder = Rotation::defaultRotation();

        FrameBuffer *mFinalFilterFramebuffer = NULL; //滤镜链的最后一个滤镜持有的FrameBuffer

        std::function<void (void)> mStartHandler = NULL;
        std::function<void (void)> mPauseHandler = NULL;
        std::function<void (void)> mFinishHandler = NULL;
        std::function<void (void)> mCancelHandler = NULL;

        void renderRecorderFramebuffer(FrameBuffer *framebuffer);
        void creatRecorderFramebuffer();
        void bindRecorderFramebuffer();
        void unBindRecorderFramebuffer();


    public:
        jobject jmediaObj = NULL;

        MediaRecorder(const EncoderConfig &config, JNIEnv * env);
        ~MediaRecorder();

        /*
         * 整个录制的每个生命周期都需要向Java层返回一个回调
         */
        void startRecording(const std::function<void ()> &handler);
        void pauseRecording(const std::function<void ()> &handler);
        void finishRecording(const std::function<void ()> &handler);
        void cancelRecording(const std::function<void ()> &handler);

        virtual void _newFrameReadyAtTime(const MediaTime &time);
        virtual void _setOutputRotation(const Rotation &rotation);
        virtual void _setOutputFramebuffer(FrameBuffer *framebuffer);
    };
}

#endif //GPUCAMERAVIDEO_MEDIARECORD_H
