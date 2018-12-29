//
// Created by 刘轩 on 2018/12/27.
//

#include "Camera.h"

static const char * vertexShader =
                "attribute vec4 aPosition;\n"
                "attribute vec4 aTexCoord;\n"
                "varying vec2 vTexCoord;\n"
                "void main() {\n"
                "   gl_Position = aPosition;\n"
                "   vTexCoord = aTexCoord;\n"
                "}\n";

static const char * fragmentShader =
                "#extension GL_OES_EGL_image_external : require\n"
                "precision mediump float;\n"
                "varying vec2 vTexCoord;\n"
                "uniform samplerExternalOES uTexture;\n"
                "void main() {\n"
                "    gl_FragColor = texture2D(uTexture, vTexCoord);\n"
                "}\n";


GCVBase::Camera::Camera() {

    runSyncContextLooper(Context::getShareContext(), [=] {
        Context::makeShareContextAsCurrent();
        mProgram = new GLProgram(vertexShader, fragmentShader);

        aPositionAttribute = mProgram->getAttributeIndex("aPosition");
        aTexCoordAttribute = mProgram->getAttributeIndex("aTexCoord");
        uTextureuniform = mProgram->getuniformIndex("uTexture");

        mProgram->useProgram();
        glEnableVertexAttribArray(aPositionAttribute);
        glEnableVertexAttribArray(aTexCoordAttribute);
    });

}

GCVBase::Camera::~Camera() {

}

const char *GCVBase::Camera::VertexShared() {
    return vertexShader;
}

const char *GCVBase::Camera::FragmentShared() {
    return fragmentShader;
}

GCVBase::EglCore *GCVBase::Camera::getEglInstance() {
    return mEglInstance;
}

void GCVBase::Camera::genSurfaceTexture() {

    if (!mEglInstance) {
        const EglCore *sharedContext = (const EglCore *)Context::getShareContext()->getEglInstance();
        mEglInstance = new EglCore( (const void *)sharedContext->getEGLContext(), NULL, 1, 1);
        mEglInstance->makeAsCurrent();
        glGenTextures(1, &mOESTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, mOESTexture);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

GLuint GCVBase::Camera::getSurfaceTexture() {
    return mOESTexture;
}

void GCVBase::Camera::surfaceTextureAvailable() {
    glFlush();

    Context::makeShareContextAsCurrent();
    mProgram->useProgram();

    glEnableVertexAttribArray(aPositionAttribute);
    glEnableVertexAttribArray(aTexCoordAttribute);

    glClearColor(0, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, mOESTexture);

    glUniform1i(uTextureuniform, 0);

    static const GLfloat vertices[] = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f,  1.0f,
            1.0f,  1.0f,
    };

    static const GLfloat texCoord[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    };

    glVertexAttribPointer(aPositionAttribute, 2, GL_FLOAT, 0, 0, vertices);
    glVertexAttribPointer(aTexCoordAttribute, 2, GL_FLOAT, 0, 0, texCoord);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(aPositionAttribute);
    glDisableVertexAttribArray(aTexCoordAttribute);
}
