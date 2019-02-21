//
// Created by 刘轩 on 2019/1/7.
//

#include "DisplayView.h"

std::string displayVertexShader =
                "attribute vec4 aPosition;\n"
                "attribute vec4 aTexCoord;\n"
                "varying vec2 vTexCoord;\n"
                "void main() {\n"
                "   gl_Position = aPosition;\n"
                "   vTexCoord = aTexCoord.xy;\n"
                "}\n";

std::string displayFragmentShader =
                "precision mediump float;\n"
                "varying vec2 vTexCoord;\n"
                "uniform sampler2D uTexture;\n"
                "void main() {\n"
                "    gl_FragColor = texture2D(uTexture, vTexCoord);\n"
                "}\n";

GCVBase::DisplayView::DisplayView(int width, int height) {
    mDisplayWidth = width;
    mDisplayHeight = height;

    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [=]{
        Context::makeShareContextAsCurrent();
        mDisplayProgram = new GLProgram(displayVertexShader, displayFragmentShader);

        if(!mDisplayProgram->isProgramLinked()){

            if(!mDisplayProgram->linkProgram()){
                // TODO 获取program连接日志`
            }
        }

        aPositionAttribute = mDisplayProgram->getAttributeIndex("aPosition");
        aTexCoordAttribute = mDisplayProgram->getAttributeIndex("aTexCoord");
        uTextureuniform = mDisplayProgram->getuniformIndex("uTexture");
    });
}

GCVBase::DisplayView::~DisplayView() {

}

void GCVBase::DisplayView::_newFrameReadyAtTime(const MediaTime &time) {
    if (!mOutputDisplayFramebuffer) {
        return;
    }

    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [=]{
        Context::makeShareContextAsCurrent();
        mDisplayProgram->useProgram();

        glViewport(0, 0, mDisplayWidth, mDisplayHeight);

        glEnableVertexAttribArray(aPositionAttribute);
        glEnableVertexAttribArray(aTexCoordAttribute);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        /**
         * 必须在这一步之前解绑 mOutputDisplayFramebuffer 的帧缓冲
         */
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mOutputDisplayFramebuffer->texture());

        static const GLfloat vertices[] = {
                -1.0f, -1.0f,
                1.0f, -1.0f,
                -1.0f,  1.0f,
                1.0f,  1.0f,
        };

        //static变量只会初始化一次，也就是说这个texCoord如果是static那么这个方法只会被调用一次！！！
        const GLfloat * texCoord = CalculateDisplayRotation(rotationDisplayView);

        glUniform1i(uTextureuniform, 1);
        glVertexAttribPointer(aPositionAttribute, 2, GL_FLOAT, 0, 0, vertices);
        glVertexAttribPointer(aTexCoordAttribute, 2, GL_FLOAT, 0, 0, texCoord);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        Context::getShareContext()->getEglInstance()->swapToScreen();

        glDisableVertexAttribArray(aPositionAttribute);
        glDisableVertexAttribArray(aTexCoordAttribute);
    });
}

void GCVBase::DisplayView::_setOutputFramebuffer(GCVBase::FrameBuffer *framebuffer) {
    mOutputDisplayFramebuffer = framebuffer;
}

void GCVBase::DisplayView::_setOutputRotation(const GCVBase::Rotation &rotation) {
    rotationDisplayView = rotation;
}

