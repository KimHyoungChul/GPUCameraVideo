//
// Created by 刘轩 on 2019/1/7.
//

#ifndef GPUCAMERAVIDEO_NATIVEOUTPUT_H
#define GPUCAMERAVIDEO_NATIVEOUTPUT_H

#include "FrameBuffer.h"

namespace GCVBase {

    class NativeOutput {

    public:

        virtual void _newFrameReadyAtTime() = 0;
        virtual void _setOutputFramebuffer(FrameBuffer *framebuffer) = 0;
    };


}


#endif //GPUCAMERAVIDEO_NATIVEOUTPUT_H
