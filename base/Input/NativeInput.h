//
// Created by 刘轩 on 2019/1/7.
//

#ifndef GPUCAMERAVIDEO_NATIVEINPUT_H
#define GPUCAMERAVIDEO_NATIVEINPUT_H

#include <vector>
#include "NativeOutput.h"
#include "FrameBuffer.h"

namespace GCVBase {

    class NativeInput {

    protected:
        std::vector<NativeOutput *> mTargets; //这里已经创建了一个 std::vector<NativeOutput *> 对象

        FrameBuffer * mOutputFrameBuffer = NULL; // TODO FrameBuffer引用需要优化为智能指针
        TextureOptions mOutputTextureOptions;

    public:
        virtual FrameBuffer * getOutputFrameBuffer();
        virtual void removeOutputFrameBuffer();

        virtual void addTarget(NativeOutput *newTarget);
        virtual const std::vector<const NativeOutput * > & getTargets();

        virtual void removeTarget(NativeOutput *target);
        virtual void removeAllTargets();
    };
}


#endif //GPUCAMERAVIDEO_NATIVEINPUT_H
