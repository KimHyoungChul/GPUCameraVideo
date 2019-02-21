//
// Created by 刘轩 on 2019/1/7.
//
#include <Context.h>
#include "NativeInput.h"

GCVBase::FrameBuffer * GCVBase::NativeInput::getOutputFrameBuffer() {
    return mOutputFrameBuffer;
}

void GCVBase::NativeInput::removeOutputFrameBuffer() {
    mOutputFrameBuffer = NULL;
}

void GCVBase::NativeInput::addTarget(GCVBase::NativeOutput *newTarget) {

    runSyncContextLooper(Context::getShareContext()->getContextLooper(),[=]{
        mTargets.push_back(newTarget);
    });
}

const std::vector<const GCVBase::NativeOutput *> & GCVBase::NativeInput::getTargets() {
    return * (const std::vector<const GCVBase::NativeOutput *> * ) & mTargets;
}

void GCVBase::NativeInput::removeTarget(GCVBase::NativeOutput *target) {

}

void GCVBase::NativeInput::removeAllTargets() {

}
