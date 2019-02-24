//
// Created by 刘轩 on 2019/1/11.
//

#ifndef GPUCAMERAVIDEO_MEDIAENCODER_H
#define GPUCAMERAVIDEO_MEDIAENCODER_H

#include <media/NdkMediaCodec.h>
#include <media/NdkMediaMuxer.h>
#include <android/log.h>
#include "Encoder.hpp"
#include "Looper.h"
#include "EncoderConfig.hpp"

namespace GCVBase {

    class MediaEncoder : Encoder {

    private:
        bool mStop = true;
        bool mIsYUV420SP = false;

        Looper * encoderLooper = nullptr;
        EncoderConfig mEncoderConfig;

        uint64_t startTime = 0;
        uint64_t writeSampleDataTime = 0;

        AMediaCodec *mVideoMediaCodec = nullptr;
        AMediaCodec *mAudioMediaCodec = nullptr;
        AMediaMuxer *mMuxer = nullptr;
        ssize_t mVideoTrackIndex = -1;
        ssize_t mAudioTrackIndex = -1;
        FILE * saveFile = nullptr;

        bool mVideoIsWritten = false; //必须先让视频帧先写入，不然开始会黑屏的
        bool mIsRunning = false;
        int mStartCount = 0;

        void initVideoCodec();
        void initAudioCodec();

        void startMediaMuxer();
        void stopMediaMuxer(AMediaCodec *codec);

    public:
        MediaEncoder(const GCVBase::EncoderConfig &config);
        ~MediaEncoder();

        void startEncoder(std::function<void(void)> startHandler = nullptr);
        void pauseEncoder(std::function<void(void)> pauseHandler = nullptr);
        void cancelEncoder(std::function<void(void)> cancelHandler = nullptr);
        void finishEncoder(std::function<void(void)> finishHandler = nullptr);

        template <class T_> void newFrameReadyAtTime(MediaBuffer<T_> * mediaBuffer){
            if(mStop){
                return;
            }

            if (mediaBuffer->mediaType == MediaType::Audio) {
                newFrameEncodeAudio(mediaBuffer);
            }

            if (mediaBuffer->mediaType == MediaType::Video) {
                newFrameEncodeVideo(mediaBuffer);
            }
        }

        void newFrameEncodeAudio(MediaBuffer<uint8_t *> *audioBuffer); //音频数据
        void newFrameEncodeVideo(MediaBuffer<uint8_t *> *videoBuffer); //视频数据

        void recordCodecBuffer(AMediaCodecBufferInfo *bufferInfo, AMediaCodec *codec, int trackIndex);
    };
}


#endif //GPUCAMERAVIDEO_MEDIAENCODER_H
