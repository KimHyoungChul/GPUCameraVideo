//
// Created by 刘轩 on 2019/1/11.
//

#include <android/log.h>
#include <Context.h>
#include "libyuv.h"
#include "MediaEncoder.hpp"

const int COLOR_FormatYUV420Planar = 0x00000013;
const int COLOR_FormatYUV420SemiPlanar = 0x00000015;

const int BUFFER_FLAG_CODEC_CONFIG = 0x00000002; // 0x00000002 = BUFFER_FLAG_CODEC_CONFIG

static inline uint64_t getCurrentTime() {
    return (uint64_t) (GCVBase::currentTimeOfNanoseconds() / 1000UL);
}

GCVBase::MediaEncoder::MediaEncoder(const GCVBase::EncoderConfig &config) {
    mEncoderConfig = config;
    encoderLooper = new Looper("encoderLooper");
    initVideoCodec();
    initAudioCodec();
}

GCVBase::MediaEncoder::~MediaEncoder() {
    delete encoderLooper;
}

/****************************************** Encoder 类生命周期函数 *******************************************/

void GCVBase::MediaEncoder::startEncoder(std::function<void()> startHandler) {
    mStop = false;

    startMediaMuxer();

    if (startHandler) {
        startHandler();
    }
}

void GCVBase::MediaEncoder::pauseEncoder(std::function<void()> pauseHandler) {
    mStop = true;
    if (pauseHandler) {
        pauseHandler();
    }
}

void GCVBase::MediaEncoder::cancelEncoder(std::function<void()> cancelHandler) {
    if (!mStop) {
        mStop = true;
        finishEncoder(nullptr);

        if (cancelHandler) {
            cancelHandler();
        }
    }
}

void GCVBase::MediaEncoder::finishEncoder(std::function<void()> finishHandler) {

    if (mAudioMediaCodec) {
        stopMediaMuxer(mAudioMediaCodec);
    }

    if (mVideoMediaCodec) {
        stopMediaMuxer(mVideoMediaCodec);
    }

    if (!mStop) {
        mStop = true;
        if (finishHandler) {
            finishHandler();
        }
    }
}
/*******************************************************************************************************/


/****************************************** Codec 生命周期函数 *******************************************/

void GCVBase::MediaEncoder::initVideoCodec() {

    if (!mEncoderConfig.colorYUVFormat) { //我们会在创建MediaRecoder时传入指定的纹理格式，这里我们设置解码的YUV数据格式缺省值为420p
        mEncoderConfig.colorYUVFormat = COLOR_FormatYUV420Planar;
    }
    mIsYUV420SP = mEncoderConfig.colorYUVFormat == COLOR_FormatYUV420SemiPlanar;

    AMediaFormat *videoEncoderFormat = AMediaFormat_new();
    AMediaFormat_setInt32(videoEncoderFormat, AMEDIAFORMAT_KEY_BIT_RATE, (int32_t) mEncoderConfig.videoBitRate);
    AMediaFormat_setInt32(videoEncoderFormat, AMEDIAFORMAT_KEY_FRAME_RATE, mEncoderConfig.frameRate);
    AMediaFormat_setInt32(videoEncoderFormat, AMEDIAFORMAT_KEY_COLOR_FORMAT, mEncoderConfig.colorYUVFormat);
    AMediaFormat_setInt32(videoEncoderFormat, AMEDIAFORMAT_KEY_WIDTH, (int32_t) mEncoderConfig.outputSize.width);
    AMediaFormat_setInt32(videoEncoderFormat, AMEDIAFORMAT_KEY_HEIGHT, (int32_t) mEncoderConfig.outputSize.height);
    AMediaFormat_setInt32(videoEncoderFormat, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, 0);
    AMediaFormat_setInt32(videoEncoderFormat, "bitrate-mode", 0); //默认比特率模式为BITRATE_MODE_CQ: 表示完全不控制码率，尽最大可能保证图像质量

    std::string videoMimeTypes[] = {"video/avc"};
    int count = sizeof(videoMimeTypes) / sizeof(const std::string);
    for (int i = 0; i < count; ++i) {
        auto videoMimeType = videoMimeTypes[i];
        mVideoMediaCodec = AMediaCodec_createEncoderByType(videoMimeType.c_str());
        AMediaFormat_setString(videoEncoderFormat, AMEDIAFORMAT_KEY_MIME, videoMimeType.c_str());

        if (mVideoMediaCodec) {
            break;
        }
    }

    int ret = AMediaCodec_configure(mVideoMediaCodec, videoEncoderFormat, NULL, NULL, AMEDIACODEC_CONFIGURE_FLAG_ENCODE); //最后一个参数表示这是个编码器,如果是解码器的话直接传个0就行了

    if (ret != AMEDIA_OK) { //当 BITRATE_MODE_CQ为不支持的比特率模式时，重新用上层传下来的bitRateMode来配置，上层是经过 MediaCodecInfo.CodecCapabilities 查询之后选取的
        AMediaFormat_setInt32(videoEncoderFormat, "bitrate-mode", mEncoderConfig.bitRateMode);
        ret = AMediaCodec_configure(mVideoMediaCodec, videoEncoderFormat, NULL, NULL, AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    }

    if (ret != AMEDIA_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "VideoCodec", "configure video encoder failed == %d", ret);
        AMediaFormat_delete(videoEncoderFormat);
    }
}

void GCVBase::MediaEncoder::initAudioCodec() {
    AMediaFormat *audioEncoderFormat = AMediaFormat_new();
    AMediaFormat_setInt32(audioEncoderFormat, AMEDIAFORMAT_KEY_SAMPLE_RATE, mEncoderConfig.sampleRate);
    AMediaFormat_setInt32(audioEncoderFormat, AMEDIAFORMAT_KEY_CHANNEL_COUNT, mEncoderConfig.getChannelCount());
    AMediaFormat_setInt32(audioEncoderFormat, AMEDIAFORMAT_KEY_BIT_RATE, (int) mEncoderConfig.audioBitRate);

    __android_log_print(ANDROID_LOG_ERROR, "AudioCodec", "audio encode mime = %s", AMediaFormat_toString(audioEncoderFormat));

    std::string audioMimeTypes[] = {"audio/mp4a-latm"};
    int count = sizeof(audioMimeTypes) / sizeof(const std::string);
    for (int i = 0; i < count; ++i) {
        auto audioMimeType = audioMimeTypes[i];

        __android_log_print(ANDROID_LOG_ERROR, "AudioCodec", "audio encode mime = %s", audioMimeType.c_str());

        mAudioMediaCodec = AMediaCodec_createEncoderByType(audioMimeType.c_str());
        AMediaFormat_setString(audioEncoderFormat, AMEDIAFORMAT_KEY_MIME, audioMimeType.c_str());

        if (mAudioMediaCodec) {
            break;
        }
    }

    int ret = AMediaCodec_configure(mAudioMediaCodec, audioEncoderFormat, NULL, NULL, AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    if (ret != AMEDIA_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "AudioCodec", "configure video encoder failed == %d", ret);
        AMediaFormat_delete(audioEncoderFormat);
    }
}

void GCVBase::MediaEncoder::newFrameEncodeVideo(GCVBase::MediaBuffer<uint8_t *> *videoBuffer) {
    if (!mVideoMediaCodec) {
        return;
    }

    auto inputBuffer = AMediaCodec_dequeueInputBuffer(mVideoMediaCodec, 2000);
    if (inputBuffer > AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
        size_t outputSize = 0; //inputBuffer的长度
        uint8_t *input = AMediaCodec_getInputBuffer(mVideoMediaCodec, (size_t) inputBuffer, &outputSize); //这玩意是送进编码器的空buffer，需要我们手动填充数据
        memset(input, 0, outputSize); //初始化inputBuffer

        int width = (int) mEncoderConfig.outputSize.width; //图像宽度（像素）
        int height = (int) mEncoderConfig.outputSize.height; //图像高度（像素）

        auto y_size = width * height;
        auto yuv_size = y_size * 3 / 2;

        //如果是420p的数据，直接ABGRToI420即可；如果是420sp的数据，则先转成420p(I420),再转成420sp(NV21)，因为libyuv不能直接将I420转到NV21
        unsigned char *I420 = mIsYUV420SP ? (unsigned char *) malloc((size_t) yuv_size) : input;
        //这里的YUV代表的是起始地址，Y = I420表示的是从I420起始的地方开始读，同理易得U、V
        unsigned char *Y = I420;
        unsigned char *U = I420 + (unsigned long) (y_size);
        unsigned char *V = U + (unsigned long) (y_size / 4);

        /* BGRAToI420, 内存顺序是BGRA,但用libyuv转化的时候用得反过来用ARGB。
         *
         * 跨距就是指图像中的一行图像数据所占的存储空间的长度，它是一个大于等于图像宽度的内存对齐的长度。
         * 这样每次以行为基准读取数据的时候就能内存对齐，虽然可能会有一点内存浪费，但是在内存充裕的今天已经无所谓了。
         */
        libyuv::ABGRToI420(videoBuffer->mediaData, //一帧的数据
                           width * 4, //一帧数据的stride(跨距)
                           Y, //Y数据的起始地址
                           width, //Y数据的 stride
                           U, //U数据的起始地址
                           width / 2, //U数据的 stride
                           V, //V数据的起始地址
                           width / 2, //V数据的 stride
                           width, //宽
                           height); //高

        if (mIsYUV420SP) {
            uint8_t *dst_y = input;
            int dst_stride_y = width;
            int dst_stride_uv = width;
            uint8_t *dst_uv = dst_y + width * height;
            libyuv::I420ToNV21(Y, width,
                               V, width / 2,
                               U, width / 2,
                               dst_y, dst_stride_y,
                               dst_uv, dst_stride_uv,
                               width, height);
            free(I420);
        }

        uint64_t time = (uint64_t) getCurrentTime();
        AMediaCodec_queueInputBuffer(mVideoMediaCodec, (size_t) inputBuffer, 0, (size_t) outputSize, time, 0);

        AMediaCodecBufferInfo bufferInfo;
        recordCodecBuffer(&bufferInfo, mVideoMediaCodec, mVideoTrackIndex);
    }
}

void GCVBase::MediaEncoder::newFrameEncodeAudio(GCVBase::MediaBuffer<uint8_t *> *audioBuffer) {
    if (!mAudioMediaCodec) {
        return;
    }

    auto buffer = audioBuffer;
    auto inputBuffer = AMediaCodec_dequeueInputBuffer(mAudioMediaCodec, 0);
    if (inputBuffer > AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
        size_t outputSize = 0;
        uint8_t *input = AMediaCodec_getInputBuffer(mAudioMediaCodec, (size_t)inputBuffer, &outputSize);
        memset(input, 0, outputSize);
        memcpy(input, buffer->mediaData, (size_t) outputSize);

        uint64_t time = (uint64_t)getCurrentTime();
        AMediaCodec_queueInputBuffer(mAudioMediaCodec, (size_t)inputBuffer, 0, (size_t) outputSize, time, 0);
        AMediaCodecBufferInfo bufferInfo;
        recordCodecBuffer(&bufferInfo, mAudioMediaCodec, mAudioTrackIndex);
    }
}

void GCVBase::MediaEncoder::recordCodecBuffer(AMediaCodecBufferInfo *bufferInfo, AMediaCodec *codec, int trackIndex) {

    auto outputBuffer = AMediaCodec_dequeueOutputBuffer(codec, bufferInfo, 2000);

    if (outputBuffer == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
        auto outputFormat = AMediaCodec_getOutputFormat(codec);
        __android_log_print(ANDROID_LOG_INFO, "outputBuffer", "AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED");
        __android_log_print(ANDROID_LOG_INFO, "AMediaCodec_getOutputFormat", "output format === %s", AMediaFormat_toString(outputFormat));

        ssize_t &index = codec == mVideoMediaCodec ? mVideoTrackIndex : mAudioTrackIndex;
        ssize_t track = AMediaMuxer_addTrack(mMuxer, outputFormat);

        if (track < 0) {
            index = mStartCount;
        } else {
            index = track;
        }
        mStartCount++;
        __android_log_print(ANDROID_LOG_INFO, "AMediaMuxer_addTrack", "add %s trackIndex: %d", mVideoMediaCodec == codec ? "video" : "Audio", index);

        if (mStartCount == 1) { //添加两次轨道之后就开始写入数据了（默认一条音轨一条视频轨道）
            int result = AMediaMuxer_start(mMuxer);
            if (result != AMEDIA_OK) {
                __android_log_print(ANDROID_LOG_ERROR, "AMediaMuxer_start", "AMediaMuxer start filed");
            }
            mIsRunning = true;
        } else {
            AMediaCodecBufferInfo buffer;
            recordCodecBuffer(&buffer, codec == mVideoMediaCodec ? mAudioMediaCodec : mVideoMediaCodec, mStartCount);
        }

    } else if (outputBuffer == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
        __android_log_print(ANDROID_LOG_INFO, "outputBuffer", "AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED");
    } else if (outputBuffer == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
        __android_log_print(ANDROID_LOG_INFO, "outputBuffer", "AMEDIACODEC_INFO_TRY_AGAIN_LATER");
    } else {
        if (!mIsRunning || (!mVideoIsWritten && trackIndex ==  mAudioTrackIndex)) { //如果视频帧还没有写入（且这一帧为音频帧）就先丢弃这一帧
            auto result = AMediaCodec_releaseOutputBuffer(codec, (size_t) outputBuffer, false);
            if (result != AMEDIA_OK) {
                __android_log_print(ANDROID_LOG_ERROR, "releaseOutputBuffer", "AMediaCodec_releaseOutputBuffer failed type:%s", trackIndex == 0 ? "audio" : "video");
            }
            return;
        }

        size_t outputSize = 0;
        auto output = AMediaCodec_getOutputBuffer(codec, (size_t)outputBuffer, &outputSize);

        if ((bufferInfo->flags & BUFFER_FLAG_CODEC_CONFIG) != 0) {
            __android_log_print(ANDROID_LOG_INFO, "getOutputBuffer", "ignoring BUFFER_FLAG_CODEC_CONFIG");
            bufferInfo->size = 0;
        }

        GCVBase::runSyncContextLooper(encoderLooper, [=]{
            if (bufferInfo->size > 0) {
//                bufferInfo->presentationTimeUs = (uint64_t)getCurrentTime();
                media_status_t status = AMediaMuxer_writeSampleData(mMuxer, trackIndex, output, bufferInfo); //这里直接就把 outputbuffer传进去了，毕竟是同步写入文件，写完后才会释放buffer
                if (status != AMEDIA_OK) {
                    __android_log_print(ANDROID_LOG_ERROR, "writeSampleData", "write sample data failed !!!type:%s", trackIndex == 0 ? "audio" : "video");
                }
                mVideoIsWritten = true;
            }

            auto result = AMediaCodec_releaseOutputBuffer(codec, (size_t)outputBuffer, false);
            if (result != AMEDIA_OK) {
                __android_log_print(ANDROID_LOG_ERROR, "releaseOutputBuffer", "AMediaCodec_releaseOutputBuffer failed type:%s", trackIndex == 0 ? "audio" : "video");
            }
        });
    }
}

void GCVBase::MediaEncoder::startMediaMuxer() {
    FILE *file = fopen(mEncoderConfig.outputPath.c_str(), "wb+");
    saveFile = file;
    int fd = fileno(file);
    mMuxer = AMediaMuxer_new(fd, AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);
    AMediaMuxer_setOrientationHint(mMuxer, mEncoderConfig.outputOrientation);

    int result = 0;
    if (mVideoMediaCodec) {
        result = AMediaCodec_start(mVideoMediaCodec);
        if (result != AMEDIA_OK) {
            __android_log_print(ANDROID_LOG_ERROR, "AMediaCodec_start", "VideoMediaCodec start filed");
        }
    }

    if (mAudioMediaCodec) {
        result = AMediaCodec_start(mAudioMediaCodec);
        if (result != AMEDIA_OK) {
            __android_log_print(ANDROID_LOG_ERROR, "AMediaCodec_start", "AudioMediaCodec start filed");
        }
    }
}

void GCVBase::MediaEncoder::stopMediaMuxer(AMediaCodec *codec) {
    if (!codec) {
        return;
    }

    std::string nowcodec = codec == mVideoMediaCodec ? "VideoMediaCodec" : "AudioMediaCodec";
    __android_log_print(ANDROID_LOG_ERROR, "stopMediaMuxer", "now Codec is %s", nowcodec.c_str());

    int result = AMediaCodec_flush(codec);
    if (result != AMEDIA_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "Codec_flush", "MediaCodec flush filed");
        return;
    }

    result = AMediaCodec_stop(codec);
    if (result != AMEDIA_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "Codec_stop", "MediaCodec stop filed");
        return;
    }

    result = AMediaCodec_delete(codec);
    if (result != AMEDIA_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "Codec_delete", "MediaCodec delete filed");
        return;
    }

    nowcodec == "VideoMediaCodec" ? mVideoMediaCodec = nullptr : mAudioMediaCodec = nullptr;
    if (mVideoMediaCodec || mAudioMediaCodec) {
        return;
    }

    if (mMuxer != nullptr) {
        int res = AMediaMuxer_stop(mMuxer);
        if (res != AMEDIA_OK) {
            __android_log_print(ANDROID_LOG_ERROR, "AMediaMuxer_stop", "AMediaMuxer_stop failed === %d", res);
        }

        res = AMediaMuxer_delete(mMuxer);
        if (res != AMEDIA_OK) {
            __android_log_print(ANDROID_LOG_ERROR, "AMediaMuxer_delet", "AMediaMuxer_delet failed === %d", res);
        }
        mMuxer = nullptr;
    }

    if (saveFile != nullptr) {
        fflush(saveFile);
        fclose(saveFile);
        saveFile = nullptr;
    }

    mIsRunning = false;

    mAudioTrackIndex = -1;
    mVideoTrackIndex = -1;
}