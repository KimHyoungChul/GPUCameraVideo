//
// Created by 刘轩 on 2019/1/11.
//

#ifndef GPUCAMERAVIDEO_MEDIABUFFER_H
#define GPUCAMERAVIDEO_MEDIABUFFER_H

#include <stdio.h>
#include <unordered_map>
#include "Time.hpp"

typedef std::string MetaDataKey;

const MetaDataKey RotationKey = "Rotation";
const MetaDataKey IsVideoKey = "IsVideo";
const MetaDataKey WidthKey = "Width";
const MetaDataKey HeightKey = "Height";
const MetaDataKey PixelFormatKey = "PixelFormat";
const MetaDataKey AudioChannelKey = "AudioChannel";
const MetaDataKey FrameSize = "FrameSize";
const MetaDataKey SampleRateKey = "SampleRate";
const MetaDataKey SampleFormatKey = "SampleFormat";

namespace GCVBase {

    enum MediaType{
        Audio = 0,
        Video = 1,
    };

    enum SampleFormat{
        S16 = 0, //有符号16位
        U8 , //无符号8位
        FLTP, //float
        ZERO, //Zero
    };

    enum AudioChannel{
        SINGLE = 0, //单通道
        DOUBLE,
    };

    template <class T_>
    struct MediaBuffer {
    public:
        T_ mediaData;
        MediaTime time = MediaTime::Zero();
        MediaType mediaType = MediaType::Audio;
        std::unordered_map<MetaDataKey, const void *> metaData;
    };


    static inline uint8_t getChannelCount(AudioChannel channel){
        uint8_t count = 0;
        switch(channel){
            case SINGLE:
                count = 1;
                break;
            case DOUBLE:
                count = 2;
                break;
            default:
                break;
        }
        return count;
    }
}

#endif //GPUCAMERAVIDEO_MEDIABUFFER_H
