//
// Created by 刘轩 on 2019/1/27.
//

#ifndef GPUCAMERAVIDEO_FILTERGROUP_H
#define GPUCAMERAVIDEO_FILTERGROUP_H

#include "NativeInput.h"
#include "Filter.h"

namespace GCVBase {

    /**
     * 这个NativeOutput 其实就相当于Java中的接口，我们在其中只写了很多抽象方法
     */
    class FilterGroup: NativeInput, NativeOutput {

    public:

        FilterGroup();
        ~FilterGroup();

        void addFilter(Filter * filter);
        void removeFilter(Filter * filter);

        void _setOutputRotation(const Rotation &rotation) override;
        void _setOutputFramebuffer(FrameBuffer *framebuffer) override;
        void _newFrameReadyAtTime() override;

    private:

        Rotation rotationFilterGroup = Rotation::defaultRotation();

        std::vector<Filter * > mFilterGroup;

        FrameBuffer *mOutputFilterGroupFramebuffer = nullptr;
    };
}


#endif //GPUCAMERAVIDEO_FILTERGROUP_H
