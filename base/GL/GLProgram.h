//
// Created by 刘轩 on 2018/12/27.
//

#ifndef GPUCAMERAVIDEO_GLPROGRAM_H
#define GPUCAMERAVIDEO_GLPROGRAM_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <string>

namespace GCVBase {

    class GLProgram {

    private:
        GLuint mProgram;
        GLuint mVertShader;
        GLuint mFragShader;

        std::string mProgramLinkLog;
        std::string mVertexCompileLog;
        std::string mFragmentCompileLog;

        bool isLinked = false;

        bool compileShader(GLuint *shader, GLenum type, const std::string &shaderString);

    public:

        GLProgram(std::string &vertexShaderString, std::string &fragmentShaderString);

        bool linkProgram();
        void useProgram();

        const std::string vertexCompileLog(){
            return mVertexCompileLog;
        }
        const std::string &fragmentCompileLog(){
            return mFragmentCompileLog;
        }
        const std::string &programLinkLog(){
            return mProgramLinkLog;
        }

        bool isProgramLinked(){
            return isLinked;
        }

        GLuint getProgram(){
            return mProgram;
        }
        GLuint getAttributeIndex(const std::string &attribute);
        GLuint getuniformIndex(const std::string &uniformName);

    };
}


#endif //GPUCAMERAVIDEO_GLPROGRAM_H
