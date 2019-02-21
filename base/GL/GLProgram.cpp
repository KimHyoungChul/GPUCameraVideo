//
// Created by 刘轩 on 2018/12/27.
//
#include <thread>
#include "GLProgram.h"

GCVBase::GLProgram::GLProgram(const std::string &vertexShaderString, const std::string &fragmentShaderString) {

    mProgram = glCreateProgram();

    if(!compileShader(&mVertShader, GL_VERTEX_SHADER, vertexShaderString)){
        // TODO 上报顶点着色器编译错误
    }

    if(!compileShader(&mFragShader, GL_FRAGMENT_SHADER, fragmentShaderString)){
        // TODO 上报片段着色器编译错误
    }

    glAttachShader(mProgram, mVertShader);
    glAttachShader(mProgram, mFragShader);
}

bool GCVBase::GLProgram::compileShader(GLuint *shader, GLenum type, const std::string &shaderString) {

    *shader = glCreateShader(type);
    if(*shader == 0){
        GLenum err = glGetError();
    }

    const GLchar *source = shaderString.c_str();
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);

    GLint compileSatuts;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compileSatuts); //查询编译成功与否的信息

    if(compileSatuts != GL_TRUE){
        GLint compileLogLength = 0;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &compileLogLength); //查询编译日志长度

        if(compileLogLength>0){
            GLchar *compileLog = new GLchar[compileLogLength];
            glGetShaderInfoLog(*shader, compileLogLength, &compileLogLength, compileLog);
            char logBuffer[compileLogLength];
            sprintf(logBuffer,"%s", compileLog);
            if (*shader == mVertShader) {
                mVertexCompileLog = logBuffer;
            } else {
                mFragmentCompileLog = logBuffer;
            }

            delete [] compileLog;
        }

        return false;
    }
    return true;
}

bool GCVBase::GLProgram::linkProgram() {
    glLinkProgram(mProgram);

    GLint linkStatus;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE) {
        GLint linkLogLength;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &linkLogLength);

        if (linkLogLength > 0) {
            GLchar *log =  new GLchar[linkLogLength];
            glGetProgramInfoLog(mProgram, linkLogLength, &linkLogLength, log);
            char logBuffer[linkLogLength];
            sprintf(logBuffer,"%s", log);
            mProgramLinkLog = logBuffer;
            delete [] log;
        }

        return false;
    }
    isLinked = true;

    return true;
}

GLuint GCVBase::GLProgram::getuniformIndex(const std::string &uniformName) {
    return (GLuint) glGetUniformLocation(mProgram, uniformName.c_str());
}

GLuint GCVBase::GLProgram::getAttributeIndex(const std::string &attribute) {
    return (GLuint) glGetAttribLocation(mProgram, attribute.c_str());
}

void GCVBase::GLProgram::useProgram() {
    glUseProgram(mProgram);
}





