//
// Created by 刘轩 on 2018/12/30.
//

#ifndef GPUCAMERAVIDEO_FRAMEBUFFER_H
#define GPUCAMERAVIDEO_FRAMEBUFFER_H

#include <Size.hpp>
#include <Context.h>
#include "TextureOptions.h"

namespace GCVBase {
    class FrameBuffer {

    private:
        GLuint mTexture = 0;
        GLuint mFrameBuffer = 0;

        Size mSize = Size::zero();

        TextureOptions mTextureOptions;

        Context * mContext = NULL;

    public:
        FrameBuffer(const Size &size = Size::zero(), const TextureOptions &options = TextureOptions(),
                    Context * context = NULL);

        void bindFramebuffer();
        void unbindFramebuffer();

        const TextureOptions &textureOptions() const;
        const Context *context() const;
        int framebuffer() const;
        GLuint texture() const;
    };
}


#endif //GPUCAMERAVIDEO_FRAMEBUFFER_H
