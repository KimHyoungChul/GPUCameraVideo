//
// Created by 刘轩 on 2019/1/27.
//

#include <Context.h>
#include "Filter.h"

std::string defaultFilterVertexShader =
        "attribute vec4 aPosition;\n"
                "attribute vec4 aTexCoord;\n"
                "varying vec2 vTexCoord;\n"
                "void main() {\n"
                "   gl_Position = aPosition;\n"
                "   vTexCoord = aTexCoord.xy;\n"
                "}\n";

std::string defaultFilterFragmentShader =
        "precision mediump float;\n"
                "varying vec2 vTexCoord;\n"
                "uniform sampler2D uTexture;\n"
                "void main() {\n"
                "    gl_FragColor = texture2D(uTexture, vTexCoord);\n"
                "}\n";

GCVBase::Filter::Filter(int width, int height) {
    Filter(defaultFilterVertexShader, defaultFilterFragmentShader, width, height);
}

GCVBase::Filter::Filter(const std::string &vertexShader, const std::string &fragmentShader, int width, int height) {

    mFilterWidth = width;
    mFilterHeight = height;

    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [=]{
        Context::makeShareContextAsCurrent();

        mFilterProgram = new GLProgram(vertexShader, fragmentShader);

        if(!mFilterProgram->isProgramLinked()){
            if(!mFilterProgram->linkProgram()){
                // TODO 获取program连接日志`
            }
        }

        aPositionAttribute = mFilterProgram->getAttributeIndex("aPosition");
        aTexCoordAttribute = mFilterProgram->getAttributeIndex("aTexCoord");
        uTextureuniform = mFilterProgram->getuniformIndex("uTexture");
    });
}


GCVBase::Filter::~Filter() {

}

void GCVBase::Filter::newFrameReadyAtTime() {
    if (!mFilterFramebuffer) {
        return;
    }

    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [=]{
        Context::makeShareContextAsCurrent();
        mFilterProgram->useProgram();

//        glViewport(0, 0, mFilterWidth, mFilterHeight);  //TODO 这个 glViewport 到底有没有用？

        glEnableVertexAttribArray(aPositionAttribute);
        glEnableVertexAttribArray(aTexCoordAttribute);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        /**
         * 必须在这一步之前解绑 mOutputDisplayFramebuffer 的帧缓冲
         */
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mFilterFramebuffer->texture());

        static const GLfloat vertices[] = {
                -1.0f, -1.0f,
                1.0f, -1.0f,
                -1.0f,  1.0f,
                1.0f,  1.0f,
        };

        static const GLfloat texCoord[] = { // Filter只负责对纹理的像素做采样处理，但对纹理坐标同样不做任何处理
                0.0f, 0.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
        };

        glUniform1i(uTextureuniform, 2);
        glVertexAttribPointer(aPositionAttribute, 2, GL_FLOAT, 0, 0, vertices);
        glVertexAttribPointer(aTexCoordAttribute, 2, GL_FLOAT, 0, 0, texCoord);

        mFilterFramebuffer->bindFramebuffer();

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        mFilterFramebuffer->unbindFramebuffer();

        glDisableVertexAttribArray(aPositionAttribute);
        glDisableVertexAttribArray(aTexCoordAttribute);
    });
}

void GCVBase::Filter::setPreviousFramebuffer(GCVBase::FrameBuffer *framebuffer) {
    mFilterFramebuffer = framebuffer;
}




