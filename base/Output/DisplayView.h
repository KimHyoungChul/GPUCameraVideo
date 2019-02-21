//
// Created by 刘轩 on 2019/1/7.
//

#ifndef GPUCAMERAVIDEO_DISPLAYVIEW_H
#define GPUCAMERAVIDEO_DISPLAYVIEW_H

#include "NativeOutput.h"
#include "GLProgram.h"

namespace GCVBase {

    class DisplayView : NativeOutput {

    private:

        GLuint aPositionAttribute = 0;
        GLuint aTexCoordAttribute = 0;
        GLuint uTextureuniform = 0;

        int mDisplayWidth;
        int mDisplayHeight;

        Rotation rotationDisplayView = Rotation::defaultRotation();

        GLProgram * mDisplayProgram;
        FrameBuffer *mOutputDisplayFramebuffer = NULL;

    public:

        DisplayView(int width, int height);
        ~DisplayView();

        virtual void _newFrameReadyAtTime(const MediaTime &time);
        virtual void _setOutputRotation(const Rotation &rotation);
        virtual void _setOutputFramebuffer(FrameBuffer *framebuffer);

    };
}


#endif //GPUCAMERAVIDEO_DISPLAYVIEW_H
