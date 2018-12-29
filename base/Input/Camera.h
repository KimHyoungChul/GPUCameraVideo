//
// Created by 刘轩 on 2018/12/27.
//

#ifndef GPUCAMERAVIDEO_CAMERA_H
#define GPUCAMERAVIDEO_CAMERA_H

#include <Context.h>
#include <GLProgram.h>

namespace GCVBase {

    class Camera {

    private:

        GLuint aPositionAttribute = 0;
        GLuint aTexCoordAttribute = 0;
        GLuint uTextureuniform = 0;

        int mPreviewWidth = 0;
        int mPreviewHeight = 0;

        GLuint mOESTexture = 0;

        EglCore * mEglInstance = NULL;

        GLProgram * mProgram = NULL;

        static const char * VertexShared();
        static const char * FragmentShared();

    public:
        jobject javaCamera = NULL;

        Camera();
        ~Camera();

        EglCore * getEglInstance();

        void genSurfaceTexture();
        GLuint getSurfaceTexture();

        void surfaceTextureAvailable();



    };
}


#endif //GPUCAMERAVIDEO_CAMERA_H
