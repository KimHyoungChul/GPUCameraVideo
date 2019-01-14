//
// Created by 刘轩 on 2019/1/11.
//

#ifndef GPUCAMERAVIDEO_ENCODERCONFIGURATION_H
#define GPUCAMERAVIDEO_ENCODERCONFIGURATION_H

#include <string>
#include <Size.hpp>
#include "MediaBuffer.hpp"

namespace GCVBase {

    class EncoderConfig {

    public:
        int frameRate = 30;
        int outputOrientation = 0;
        int colorYUVFormat = 0;
        int bitRateMode = -1;
        int sampleRate = 44100;

        unsigned long videoBitRate = 0;
        unsigned long audioBitRate = 6400;

        Size outputSize = Size::zero();
        std::string outputPath;

        AudioChannel audioChannel = DOUBLE; //双声道
        SampleFormat sampleFormat = S16; //有符号16位

        int getChannelCount() const {
            return GCVBase::getChannelCount(audioChannel);
        }

        void setOutputSize(Size size){
            int newWidth = (int)(ceilf(size.width / 16) * 16);
            int newHeight = (int)(ceilf(size.height / 16) * 16);
            outputSize = Size(newWidth, newHeight);
        }

        Size getOutputSize(){
            return outputSize;
        }

    };
}

#endif //GPUCAMERAVIDEO_ENCODERCONFIGURATION_H
