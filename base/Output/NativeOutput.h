//
// Created by 刘轩 on 2019/1/7.
//

#ifndef GPUCAMERAVIDEO_NATIVEOUTPUT_H
#define GPUCAMERAVIDEO_NATIVEOUTPUT_H

#include "Time.hpp"
#include "FrameBuffer.h"

namespace GCVBase {

    class NativeOutput {

    public:

        virtual void _newFrameReadyAtTime(const MediaTime &time) = 0;
        virtual void _setOutputFramebuffer(FrameBuffer *framebuffer) = 0;
    };


}


#endif //GPUCAMERAVIDEO_NATIVEOUTPUT_H
