//
// Created by 刘轩 on 2019/1/27.
//

#ifndef GPUCAMERAVIDEO_FILTER_H
#define GPUCAMERAVIDEO_FILTER_H

#include <string>
#include "Rotation.hpp"
#include "Time.hpp"
#include "GLProgram.h"
#include "FrameBuffer.h"

namespace GCVBase {

    class Filter {

    public:
        Filter(int width, int height);
        Filter(const std::string & vertexShader, const std::string & fragmentShader, int width, int height);
        ~Filter();

        void setPreviousFramebuffer(FrameBuffer *framebuffer);
        void newFrameReadyAtTime();

    private:

        GLuint aPositionAttribute = 0;
        GLuint aTexCoordAttribute = 0;
        GLuint uTextureuniform = 0;

        int mFilterWidth;
        int mFilterHeight;

        GLProgram * mFilterProgram = NULL;
        FrameBuffer *mFilterFramebuffer = NULL;
    };
}


#endif //GPUCAMERAVIDEO_FILTER_H
