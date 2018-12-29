//
// Created by 刘轩 on 2018/12/27.
//

#include "GLProgram.h"

GCVBase::GLProgram::GLProgram(const char *vertexShaderString, const char *fragmentShaderString) {
    mProgram = glCreateProgram();

    if(!compileShader(mVertShader, GL_VERTEX_SHADER, vertexShaderString)){
        // TODO 上报顶点着色器编译错误
    }

    if(!compileShader(mFragShader, GL_FRAGMENT_SHADER, fragmentShaderString)){
        // TODO 上报片段着色器编译错误
    }

    glAttachShader(mProgram, mVertShader);
    glAttachShader(mProgram, mFragShader);
}

bool GCVBase::GLProgram::compileShader(GLuint shader, GLenum type, const char *shaderString) {

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderString, NULL);
    glCompileShader(shader);

    GLint compileSatuts;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSatuts); //查询编译成功与否的信息

    if(compileSatuts != GL_TRUE){
        GLint compileLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &compileLogLength); //查询编译日志长度

        char compileLog[compileLogLength];
        glGetShaderInfoLog(shader, compileLogLength, &compileLogLength, compileLog);

        if (shader == mVertShader) {
            mVertexCompileLog = compileLog;
        } else {
            mFragmentCompileLog = compileLog;
        }

        return false;
    }
    return true;
}

bool GCVBase::GLProgram::linkProgram() {
    glLinkProgram(mProgram);

    GLint linkStatus;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &linkStatus);

    if (linkStatus != GL_TRUE) {
        GLint linkLogLength;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &linkLogLength);

        char linkLog[linkLogLength];
        glGetProgramInfoLog(mProgram, linkLogLength, &linkLogLength, linkLog);

        mProgramLinkLog = linkLog;

        return false;
    }
    isLinked = true;

    return true;
}

GLuint GCVBase::GLProgram::getuniformIndex(const char * uniformName) {
    return (GLuint) glGetUniformLocation(mProgram, uniformName);
}

GLuint GCVBase::GLProgram::getAttributeIndex(const char * attribute) {
    return (GLuint) glGetAttribLocation(mProgram, attribute);
}

void GCVBase::GLProgram::useProgram() {
    glUseProgram(mProgram);
}





