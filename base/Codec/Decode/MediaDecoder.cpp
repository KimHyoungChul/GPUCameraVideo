//
// Created by 刘轩 on 2019/2/5.
//

#include "MediaDecoder.h"
#include "Rotation.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <android/log.h>

static inline uint64_t getCurrentTime() {
    return (uint64_t) (GCVBase::currentTimeOfNanoseconds() / 1000UL);
}

GCVBase::MediaDecoder::MediaDecoder(const std::string &filePath,  ANativeWindow * nativeWindow) {
    if(filePath.empty()){
        __android_log_print(ANDROID_LOG_DEBUG, "MediaDecoder init", "MediaDecoder init without filePath");
    } else{
        mFilePath = filePath;
    }

    frameAudioBuffer = new MediaBuffer<uint8_t *>();
    frameVideoBuffer = new MediaBuffer<uint8_t *>();

    yuvFrameBuffer = new YUVFrameBuffer();

    if(loadFile()){
        if(!initVideoCodec(nativeWindow)){
            // TODO 打Log
            return ;
        }

        if(!initAudioCodec()){
            // TODO 打Log
            return ;
        }
    }
}

GCVBase::MediaDecoder::~MediaDecoder() {
    if(mVideoDecodeCodec){
        AMediaExtractor_delete(videoEx);
        AMediaCodec_stop(mVideoDecodeCodec);
        AMediaCodec_delete(mVideoDecodeCodec);
    }

    if(mAudioDecodeCodec){
        AMediaExtractor_delete(audioEx);
        AMediaCodec_stop(mAudioDecodeCodec);
        AMediaCodec_delete(mAudioDecodeCodec);
    }
}


bool GCVBase::MediaDecoder::loadFile() {

    if(mFilePath.empty()){
        __android_log_print(ANDROID_LOG_ERROR, "loadFile", "loadFile failed bacause the filePath is null");
        return false;
    }
    bool reslut = false;

    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [&]{

        off_t offset = 0, fileLen = 0;

        const char *fileSTR = mFilePath.c_str();
        int fd = open(fileSTR, O_RDONLY);
        if(fd <0){
            return NULL;
        }

        fileLen = lseek(fd, 0, SEEK_END);
        if (fileLen < 0) {
            ::close(fd);
            return NULL;
        }

        (void) lseek(fd, 0, SEEK_SET);

        videoEx = AMediaExtractor_new();
        media_status_t videoStatus = AMediaExtractor_setDataSourceFd(videoEx, fd,
                                                                     static_cast<off64_t>(offset),
                                                                     static_cast<off64_t>(fileLen));
        audioEx = AMediaExtractor_new();
        media_status_t audioStatus = AMediaExtractor_setDataSourceFd(audioEx, fd,
                                                                     static_cast<off64_t>(offset),
                                                                     static_cast<off64_t>(fileLen));
        close(fd);

        if (videoStatus != AMEDIA_OK) {
            __android_log_print(ANDROID_LOG_ERROR, "setVideoDataSourceFd", "Set vedio data source error: %d", videoStatus);
            return NULL;
        }


        if (audioStatus != AMEDIA_OK) {
            __android_log_print(ANDROID_LOG_ERROR, "setAudioDataSourceFd", "Set Audio data source error: %d", audioStatus);
            return NULL;
        }

        reslut = true;

        return NULL;
    });

    return reslut;
}

bool GCVBase::MediaDecoder::initVideoCodec(ANativeWindow * nativeWindow) {
    int vedioTrackNum = AMediaExtractor_getTrackCount(videoEx);

    for (int i = 0; i < vedioTrackNum; i++) {

        AMediaFormat *vedioformat = AMediaExtractor_getTrackFormat(videoEx, i);   //分离轨道

        const char *mime;

        if (!AMediaFormat_getString(vedioformat, AMEDIAFORMAT_KEY_MIME, &mime)) {
            __android_log_print(ANDROID_LOG_DEBUG, "Vedio AMediaFormat_getString", "No this MIME type = %s", mime);
        } else if (!strncmp(mime, "video/", 6)) { //说明视频轨道是有的，分离视频轨道

            AMediaExtractor_selectTrack(videoEx, i);
            mVideoDecodeCodec = AMediaCodec_createDecoderByType(mime); //创建视频解码器
            if(nativeWindow){
                mIsSurfacePlayer = true;
                AMediaCodec_configure(mVideoDecodeCodec, vedioformat, nativeWindow, NULL, 0);
            } else {
                AMediaCodec_configure(mVideoDecodeCodec, vedioformat, NULL, NULL, 0);    // 初始化解码器，这里暂时不指定播放的window
            }
            AMediaCodec_start(mVideoDecodeCodec);   //启动解码器
        }
        AMediaFormat_delete(vedioformat);
    }

    return false;
}

// TODO 这两段逻辑可以合并一下！！！

bool GCVBase::MediaDecoder::initAudioCodec() {
    int audioTrackNum = AMediaExtractor_getTrackCount(audioEx);

    for (int i = 0; i < audioTrackNum; i++) {
        AMediaFormat *audioformat = AMediaExtractor_getTrackFormat(audioEx, i);   //分离轨道

        const char *mime;

        if (!AMediaFormat_getString(audioformat, AMEDIAFORMAT_KEY_MIME, &mime)) {
            __android_log_print(ANDROID_LOG_DEBUG, "Audio AMediaFormat_getString", "No this MIME type = %s", mime);
        } else if (!strncmp(mime, "audio/", 6)) {

            AMediaExtractor_selectTrack(audioEx, i);

            mAudioDecodeCodec = AMediaCodec_createDecoderByType(mime); //创建音频解码器
            AMediaCodec_configure(mAudioDecodeCodec, audioformat, NULL, NULL, 0);
            AMediaCodec_start(mAudioDecodeCodec);   //启动解码器
        }
        AMediaFormat_delete(audioformat);
    }

    return false;
}



GCVBase::MediaBuffer<uint8_t *> *GCVBase::MediaDecoder::decodeNewFrameAudio() {

    return nullptr;
}

GCVBase::MediaBuffer<GCVBase::FrameBuffer *> *GCVBase::MediaDecoder::decodeNewFrameVideo() {

    MediaBuffer<uint8_t *> * frameBuffer = getCodecFrameVideoBuffer();

    if(!frameBuffer){
        //TODO 报错
        return nullptr;
    }

    MediaBuffer<GCVBase::FrameBuffer *> * outputVideoBuffer = yuvFrameBuffer->decodeYUVBuffer(frameBuffer);
    return outputVideoBuffer;
}

GCVBase::MediaBuffer<uint8_t *> *GCVBase::MediaDecoder::getCodecFrameVideoBuffer() {

    ssize_t bufferIndex = -1;

    if (!sawInput) {
        bufferIndex = AMediaCodec_dequeueInputBuffer(mVideoDecodeCodec, 2000);
        if (bufferIndex >= 0) {
            size_t buffsize;

            auto buf = AMediaCodec_getInputBuffer(mVideoDecodeCodec, bufferIndex, &buffsize);
            auto samplesize = AMediaExtractor_readSampleData(videoEx, buf, buffsize);
            if (samplesize < 0) {
                __android_log_print(ANDROID_LOG_ERROR, "AMediaCodec_dequeueInputBuffer", "InputBuffer stream is end");
                samplesize = 0;
                sawInput = true;
            }

            auto presentationTimeUs = AMediaExtractor_getSampleTime(videoEx);
            AMediaCodec_queueInputBuffer(mVideoDecodeCodec, bufferIndex, 0, samplesize, presentationTimeUs, sawInput ? AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM : 0);
            AMediaExtractor_advance(videoEx);
        }
    }


    if(!sawOutput) {
        AMediaCodecBufferInfo info;
        auto index = AMediaCodec_dequeueOutputBuffer(mVideoDecodeCodec, &info, 0);

        if (index >= 0) {

            if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM ) {     //输出流结束
                __android_log_print(ANDROID_LOG_ERROR, "AMediaCodec_dequeueOutputBuffer", "OutputBuffer stream is end");
                sawOutput = true;
            }

            int64_t presentationNano = info.presentationTimeUs * 1000;
            if (renderStart < 0) {
                renderStart = (int64_t)currentTimeOfNanoseconds() - presentationNano;
            }
            int64_t delay = (renderStart + presentationNano) - (int64_t)currentTimeOfNanoseconds();
            if (delay > 0) {
                //延时操作，如果缓冲区里的可展示时间 > 当前视频播放的进度，就休眠一下
                usleep(delay / 1000);
            }

            if(!mIsSurfacePlayer) {

                /**
                 * 用AMediaCodec_getOutputFormat接口得到Format对象，并且不能设置surface数据的，此时才能得到正确的颜色空间等数据，不过这样的话这个接口设计的也是很匪夷所思
                 */
                float rotation;
                int framerate = 30, bitrate = 500000;
                int32_t colorFormat, width, height, stride;

                auto format = AMediaCodec_getOutputFormat(mVideoDecodeCodec);
                AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_COLOR_FORMAT, &colorFormat);
                AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_BIT_RATE, bitrate);
                AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_FRAME_RATE, framerate);
                AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, framerate);
                AMediaFormat_getFloat(format, "rotation", &rotation);
                AMediaFormat_getInt32(format, "width", &width);
                AMediaFormat_getInt32(format, "height", &height);
                AMediaFormat_getInt32(format, "stride", &stride);

                __android_log_print(ANDROID_LOG_DEBUG, "AMediaCodec_dequeueOutputBuffer",
                                    "color Format = %d width = %d height = %d stride = %d rotation = %d framerate = %d bitrate = %d",
                                    colorFormat, width, height, stride, (int) rotation, framerate,
                                    bitrate);

                __android_log_print(ANDROID_LOG_DEBUG, "AMediaCodec_dequeueOutputBuffer",
                                    "AMediaFormat_toString: %s", AMediaFormat_toString(format));

                AMediaFormat_delete(format);

                std::unordered_map<MetaDataKey, const void *> metaData;

                metaData[RotationKey] = (const void *) (long) rotation;
                metaData[WidthKey] = (const void *) (long) width;
                metaData[HeightKey] = (const void *) (long) height;
                metaData[PixelFormatKey] = (const void *) (long) colorFormat;

                frameVideoBuffer->mediaType = Video;
                frameVideoBuffer->metaData = metaData;

                size_t out_size;
                uint8_t * outputBuf = NULL;

                outputBuf = AMediaCodec_getOutputBuffer(mVideoDecodeCodec, index, &out_size);

                frameVideoBuffer->mediaData = outputBuf;
                frameVideoBuffer->time = *(MediaTime *) &info.presentationTimeUs;

                AMediaCodec_releaseOutputBuffer(mVideoDecodeCodec, index, false);

                return frameVideoBuffer;

            } else{

                AMediaCodec_releaseOutputBuffer(mVideoDecodeCodec, index, true); //第三个参数为true就会渲染到surface上
            }

        }else if(index == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED){    //缓冲区的数据发生变化
            __android_log_print(ANDROID_LOG_DEBUG, "AMediaCodec_dequeueOutputBuffer", "output buffers changed");

        } else if (index == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {  //format格式参数发生变化
            auto format = AMediaCodec_getOutputFormat(mVideoDecodeCodec);
            __android_log_print(ANDROID_LOG_DEBUG, "AMediaCodec_dequeueOutputBuffer", "format changed to: %s", AMediaFormat_toString(format));

            AMediaFormat_delete(format);
        } else if (index == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {    //当前没有数据输出，稍后重试
            __android_log_print(ANDROID_LOG_DEBUG, "AMediaCodec_dequeueOutputBuffer", "no output buffer right now");

        } else {
            mIsFinished = true;
            __android_log_print(ANDROID_LOG_DEBUG, "AMediaCodec_dequeueOutputBuffer", "unexpected info code: %zd", index);
        }
    }

    if(sawInput && sawOutput){
        mIsFinished = true;
        __android_log_print(ANDROID_LOG_DEBUG, "AMediaCodec_dequeueOutputBuffer", "decode finished");
        return nullptr;
    }

    return nullptr;
}

void GCVBase::MediaDecoder::setFilePath(const std::string &readPath) {
    mFilePath = readPath;
}

void GCVBase::MediaDecoder::setNativeSurface(ANativeWindow *nativeWindow) {
    mNativeWindow = nativeWindow;
}







