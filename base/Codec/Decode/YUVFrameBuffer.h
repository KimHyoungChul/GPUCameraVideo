//
// Created by 刘轩 on 2019/2/5.
//

#ifndef GPUCAMERAVIDEO_YUVFRAMEBUFFER_H
#define GPUCAMERAVIDEO_YUVFRAMEBUFFER_H

#include <FrameBuffer.h>
#include <Rotation.hpp>
#include <MediaBuffer.hpp>
#include <GLProgram.h>

namespace GCVBase {

    class YUVFrameBuffer {

    public:

        YUVFrameBuffer(int mFrameWidth = 0,int mFrameHeight = 0);
        ~YUVFrameBuffer();

        MediaBuffer<FrameBuffer *> * fboBuffer = nullptr;

        FrameBuffer * getyuv420Framebuffer(GLuint luma, GLuint cb, GLuint cr);

        MediaBuffer<FrameBuffer *> * decodeYUVBuffer(MediaBuffer<uint8_t *> *videoBuffer);

    private:

        int mFrameWidth = 0;
        int mFrameHeight = 0;

        FrameBuffer *mTextureY = nullptr;
        FrameBuffer *mTextureCb = nullptr;
        FrameBuffer *mTextureCr = nullptr;

        FrameBuffer *mYUVFramebuffer = nullptr;

        GLProgram *mYUVfboProgram;

        GLuint mYUVPositionAttribute;
        GLuint mYUVTexCoordAttribute;
        GLuint mYUVLuminanceTextureUniform;
        GLuint mYUVCbTextureUniform;
        GLuint mYUVCrTextureUniform;
        GLuint mYUVMatrixUniform;

        //yuv数据
        char *yBuffer;
        size_t yBufferSize;

        char *uBuffer;
        char *vBuffer;
        size_t uvBufferSize;   //uv大小是一致的

        bool checkUpdateFrameBuffer(MediaBuffer<uint8_t *> *videoBuffer);

    };
}


#endif //GPUCAMERAVIDEO_YUVFRAMEBUFFER_H
