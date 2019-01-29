//
// Created by 刘轩 on 2019/1/27.
//

#include <android/log.h>
#include "FilterGroup.h"


GCVBase::FilterGroup::FilterGroup() {

}

GCVBase::FilterGroup::~FilterGroup() {

}

void GCVBase::FilterGroup::_newFrameReadyAtTime(const GCVBase::MediaTime &time) {

    if(mOutputFilterGroupFramebuffer == NULL){
        return;
    }

    //逐个绘制Filter到 mOutputFilterGroupFramebuffer中的Texture上
    // TODO: 待优化，没必要每帧都绘制一遍，第一次绘制完保存下，之后remove改变的时候更新一遍
    for(auto i = mFilterGroup.begin(); i< mFilterGroup.end(); i++) {
        auto currentFilter = * i;
        currentFilter->setPreviousFramebuffer(mOutputFilterGroupFramebuffer);
        currentFilter->newFrameReadyAtTime();
    }

    for(auto i = mTargets.begin(); i < mTargets.end(); i++){
        auto currentTarget =  * i;
        currentTarget->_setOutputFramebuffer(mOutputFilterGroupFramebuffer);
        currentTarget->_newFrameReadyAtTime(time);
    }
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
