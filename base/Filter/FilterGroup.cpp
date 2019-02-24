//
// Created by 刘轩 on 2019/1/27.
//

#include "FilterGroup.h"


GCVBase::FilterGroup::FilterGroup() {

}

GCVBase::FilterGroup::~FilterGroup() {

}

void GCVBase::FilterGroup::_newFrameReadyAtTime() {

    if(mOutputFilterGroupFramebuffer == nullptr){
        return;
    }

    //逐个绘制Filter到 mOutputFilterGroupFramebuffer中的Texture上
    if(!mFilterGroup.empty()) {
        for (auto i = mFilterGroup.begin(); i < mFilterGroup.end(); i++) {
            auto currentFilter = *i;
            currentFilter->setPreviousFramebuffer(mOutputFilterGroupFramebuffer);
            currentFilter->newFrameReadyAtTime();
        }
    }

    for(auto i = mTargets.begin(); i < mTargets.end(); i++){
        auto currentTarget =  * i;
        currentTarget->_setOutputRotation(rotationFilterGroup);
        currentTarget->_setOutputFramebuffer(mOutputFilterGroupFramebuffer);
        currentTarget->_newFrameReadyAtTime();
    }
}

void GCVBase::FilterGroup::_setOutputRotation(const GCVBase::Rotation &mRotation) {
    rotationFilterGroup = mRotation;
}

void GCVBase::FilterGroup::_setOutputFramebuffer(GCVBase::FrameBuffer *framebuffer) {
    mOutputFilterGroupFramebuffer = framebuffer;
}

void GCVBase::FilterGroup::addFilter(GCVBase::Filter *filter) {

    runSyncContextLooper(Context::getShareContext()->getContextLooper(),[=]{
        mFilterGroup.push_back(filter);
    });
}

void GCVBase::FilterGroup::removeFilter(GCVBase::Filter *filter) {

}
