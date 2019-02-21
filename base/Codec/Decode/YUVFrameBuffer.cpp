//
// Created by 刘轩 on 2019/2/5.
//

#include "YUVFrameBuffer.h"

using namespace GCVBase;

std::string yuvFrameBufferVertexShader =
        "attribute vec4 aPosition;\n"
        "attribute vec4 aTexCoord;\n"
        "varying vec2 vTexCoord;\n"
        "void main() {\n"
        "   gl_Position = aPosition;\n"
        "   vTexCoord = aTexCoord.xy;\n"
        "}\n";

std::string yuvFrameBufferFragmentShader =
        "varying highp vec2 vTexCoord;\n"
        "uniform sampler2D luminanceTexture;\n"
        "uniform sampler2D CbTexture;\n"
        "uniform sampler2D CrTexture;\n"
        "uniform mediump mat3 colorConversionMatrix;\n"
        "void main() {\n"
        "    mediump vec3 yuv;\n"
        "    lowp    vec3 rgb;\n"
        "    \n"
        "    yuv.x = (texture2D(luminanceTexture, vTexCoord).r - (16.0 / 255.0));\n"
        "    yuv.y = (texture2D(CbTexture, vTexCoord).r - 0.5);\n"
        "    yuv.z = (texture2D(CrTexture, vTexCoord).r - 0.5);\n"
        "    rgb = colorConversionMatrix * yuv;\n"
        "    gl_FragColor = vec4(rgb, 1);\n"
        "}\n";

GCVBase::YUVFrameBuffer::YUVFrameBuffer() {

    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [=] {
        mYUVfboProgram = new GLProgram(yuvFrameBufferVertexShader, yuvFrameBufferFragmentShader);

        if(!mYUVfboProgram->isProgramLinked()){

            if(!mYUVfboProgram->linkProgram()){
                // TODO 获取program连接日志`
            }
        }

        mYUVPositionAttribute = mYUVfboProgram->getAttributeIndex("aPosition");
        mYUVTexCoordAttribute = mYUVfboProgram->getAttributeIndex("aTexCoord");

        mYUVLuminanceTextureUniform = mYUVfboProgram->getuniformIndex("luminanceTexture");
        mYUVCbTextureUniform = mYUVfboProgram->getuniformIndex("CbTexture");
        mYUVCrTextureUniform = mYUVfboProgram->getuniformIndex("CrTexture");

        mYUVMatrixUniform = mYUVfboProgram->getuniformIndex("colorConversionMatrix");
    });
}

GCVBase::YUVFrameBuffer::~YUVFrameBuffer() {

}


MediaBuffer<FrameBuffer *> *YUVFrameBuffer::decodeYUVBuffer(MediaBuffer<uint8_t *> *videoBuffer) {

    /*
     * TODO 需要时刻关注这个 buffer对象，及时释放他占用的内存！！！
     */
    auto *fboBuffer  = new MediaBuffer<FrameBuffer *>();

    if(videoBuffer->mediaType == MediaType::Video){

        int width = (int) (videoBuffer->metaData[WidthKey]);
        int height = (int) (videoBuffer->metaData[HeightKey]);
        int frameSize = width * height;     //注意YUV信号是frameSize的3/2大小

        char *frame = (char *) videoBuffer->mediaData;

        yBufferSize = static_cast<size_t>(frameSize);
        uvBufferSize = static_cast<size_t>(frameSize/4);  //u、v各占y信号的1/4大小

        yBuffer = (char *) malloc(yBufferSize);
        uBuffer = (char *) malloc(uvBufferSize);
        vBuffer = (char *) malloc(uvBufferSize);

        memset(yBuffer, 0, yBufferSize);
        memset(uBuffer, 0, uvBufferSize);
        memset(vBuffer, 0, uvBufferSize);

        memcpy(yBuffer, frame, yBufferSize);  //复制y信号数据

        int pixelFormat = (int) videoBuffer->metaData[PixelFormatKey];

        int j = 0;
        switch (pixelFormat) {
            case 21: {  //21代表420sp格式，也就是UV信号相间隔的传过来
                for (int i = yBufferSize; i < (yBufferSize + 2 * uvBufferSize);) {
                    uBuffer[j] = frame[i];  //u信号
                    vBuffer[j] = frame[i + 1];    //v信号

                    i += 2;
                    j++;
                }
            }break;

            case 19: {   //19代表420p格式，UV信号是在最后分两段整体传过来的
                for (int i = yBufferSize; i < (yBufferSize + uvBufferSize); i++) {
                    uBuffer[j] = frame[i];  //u信号
                    vBuffer[j] = frame[yBufferSize + 1];    //v信号

                    j++;
                }
            }break;

            default:
                return nullptr;
        }

        TextureOptions options;
        options.internalFormat = GL_LUMINANCE;
        options.format = GL_LUMINANCE;

        //Texture Y
        Size size(width, height); //这个size对象占用的是栈空间,方法块执行完自动释放
        mTextureY = new FrameBuffer(size, options, Context::getShareContext());
        glBindTexture(GL_TEXTURE_2D, mTextureY->texture());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, yBuffer);
        free(yBuffer);
        yBuffer = nullptr;

        //Texture U
        size =  Size(size.width / 2, size.height / 2);
        mTextureCb = new FrameBuffer(size, options, Context::getShareContext());
        glBindTexture(GL_TEXTURE_2D, mTextureCb->texture());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, uBuffer);
        free(uBuffer);
        uBuffer = nullptr;

        //Texture V
        mTextureCr = new FrameBuffer(size, options, Context::getShareContext());
        glBindTexture(GL_TEXTURE_2D, mTextureCr->texture());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, vBuffer);
        free(vBuffer);
        vBuffer = nullptr;

        FrameBuffer *framebuffer = getyuv420Framebuffer(mTextureY->texture(), mTextureCb->texture(), mTextureCr->texture(), options, size);
        fboBuffer->mediaData = framebuffer;

        delete mTextureY;
        delete mTextureCb;
        delete mTextureCr;
    }

    if(!fboBuffer->mediaData){
        return nullptr;
    }

    return fboBuffer;
}

FrameBuffer *YUVFrameBuffer::getyuv420Framebuffer(GLuint luma, GLuint cb, GLuint cr, const TextureOptions &options, const GCVBase::Size &size) {

    FrameBuffer *framebuffer = nullptr;
    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [&]{

        Context::makeShareContextAsCurrent();
        mYUVfboProgram -> useProgram();

        Size framebufferSize = size;
        framebuffer = new FrameBuffer(framebufferSize, TextureOptions(), Context::getShareContext()); // TODO 这个FrameBuffer对象也要重点关注！！！

        glEnableVertexAttribArray(mYUVPositionAttribute);
        glEnableVertexAttribArray(mYUVTexCoordAttribute);

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static const GLfloat vertices[] = {
                -1.0f, -1.0f,
                1.0f, -1.0f,
                -1.0f,  1.0f,
                1.0f,  1.0f,
        };

        static const GLfloat texCoord[] = { //这里先将纹理向左(逆时针)旋转90°，模仿相机硬件的角度，这样就可以兼容displayView中的旋转角度了
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                0.0f, 1.0f,
        };


        glVertexAttribPointer(mYUVPositionAttribute, 2, GL_FLOAT, 0, 0, vertices);
        glVertexAttribPointer(mYUVTexCoordAttribute, 2, GL_FLOAT, 0, 0, texCoord);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, luma);
        glUniform1i(mYUVLuminanceTextureUniform, 5);

        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, cb);
        glUniform1i(mYUVCbTextureUniform, 6);

        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, cr);
        glUniform1i(mYUVCrTextureUniform, 7);

        static const GLfloat colorConversionMatrix[] = {
                1.164, 1.164, 1.164,
                0.0, -0.213, 2.112,
                1.793, -0.533, 0.0,
        };

        glUniformMatrix3fv(mYUVMatrixUniform, 1, GL_FALSE, colorConversionMatrix);

        framebuffer->bindFramebuffer();

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        framebuffer->unbindFramebuffer();

        glDisableVertexAttribArray(mYUVPositionAttribute);
        glDisableVertexAttribArray(mYUVTexCoordAttribute);
    });

    return framebuffer;
}

