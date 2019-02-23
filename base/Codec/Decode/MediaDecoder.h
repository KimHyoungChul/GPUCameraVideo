//
// Created by 刘轩 on 2019/2/5.
//

#ifndef GPUCAMERAVIDEO_MEDIADECODER_H
#define GPUCAMERAVIDEO_MEDIADECODER_H

#include <string>
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaExtractor.h>
#include <MediaBuffer.hpp>
#include <FrameBuffer.h>

#include "YUVFrameBuffer.h"

namespace GCVBase {

    class MediaDecoder {

    public:
        MediaDecoder(const std::string &filePath, ANativeWindow * nativeWindow = nullptr);
        ~MediaDecoder();

        void setFilePath(const std::string &readPath);
        void setNativeSurface(ANativeWindow * nativeWindow);

        MediaBuffer<uint8_t *> * decodeNewFrameAudio(); //音频数据
        MediaBuffer<FrameBuffer *> * decodeNewFrameVideo(); //视频数据

        MediaBuffer<uint8_t *> * getCodecFrameVideoBuffer();

        bool decodingIsFinished() const {
            return mIsFinished;
        };

        bool decodIsSurfacePlayer() const {
            return mIsSurfacePlayer;
        }

    private:

        std::string mFilePath;

        uint8_t * outputBuffer;

        ANativeWindow *mNativeWindow;

        AMediaExtractor *videoEx;
        AMediaExtractor *audioEx;

        AMediaCodec *mVideoDecodeCodec = NULL;
        AMediaCodec *mAudioDecodeCodec = NULL;

        YUVFrameBuffer * yuvFrameBuffer = NULL;

        MediaBuffer<uint8_t *> * frameAudioBuffer = NULL;
        MediaBuffer<uint8_t *> * frameVideoBuffer = NULL;

        int64_t renderStartTime = -1;    //开始渲染时间，用来控制播放速率
        bool mIsSurfacePlayer = false; // 是否使用surface直接作为输出（不用gl合成）
        bool mIsFinished = false;
        bool sawInput = false;  //输入流是否结束
        bool sawOutput = false; //输出流是否结束


        bool loadFile();
        bool initVideoCodec(ANativeWindow * nativeWindow = nullptr);
        bool initAudioCodec();
    };
}


#endif //GPUCAMERAVIDEO_MEDIADECODER_H
