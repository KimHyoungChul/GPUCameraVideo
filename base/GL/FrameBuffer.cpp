//
// Created by 刘轩 on 2018/12/30.
//

#include "FrameBuffer.h"

GCVBase::FrameBuffer::FrameBuffer(const GCVBase::Size &size, const GCVBase::TextureOptions &options,
                                  GCVBase::Context *context) {
    mSize = size;
    mTextureOptions = options;
    mContext = context;

    runSyncContextLooper(mContext->getContextLooper(),[=]{
        mContext->useAsCurrentContext();

        glGenTextures(1, &mTexture);
        mTextureOptions.bindTexture(GL_TEXTURE_2D, mTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, mTextureOptions.internalFormat, mSize.width, mSize.height, 0,
                     mTextureOptions.format, mTextureOptions.type, NULL);

        glGenFramebuffers(1, &mFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(status != GL_FRAMEBUFFER_COMPLETE){
            // TODO 报错
            return;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });
}

void GCVBase::FrameBuffer::bindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
    glViewport(0, 0, mSize.width, mSize.height);
}

void GCVBase::FrameBuffer::unbindFramebuffer() {
    /*
     * 由于我们的帧缓冲不是默认的帧缓冲，渲染命令对窗口的视频输出不会产生任何影响。出于这个原因，它被称为离屏渲染，就
     * 是渲染到一个另外的缓冲中。为了后续让所有的渲染操作对主窗口产生影响我们必须通过绑定为0来使 "默认帧缓冲" 被激活
     */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


const GCVBase::TextureOptions &GCVBase::FrameBuffer::textureOptions() const {
    return mTextureOptions;
}

const GCVBase::Context *GCVBase::FrameBuffer::context() const {
    return mContext;
}

int GCVBase::FrameBuffer::framebuffer() const {
    return mFrameBuffer;
}

GLuint GCVBase::FrameBuffer::texture() const {
    return mTexture;
}





