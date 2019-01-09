//
// Created by 刘轩 on 2018/12/27.
//

#ifndef GPUCAMERAVIDEO_CAMERA_H
#define GPUCAMERAVIDEO_CAMERA_H

#include "Context.h"
#include "GLProgram.h"
#include "NativeInput.h"

namespace GCVBase {

    class Camera : NativeInput{

    private:

        GLuint aPositionAttribute = 0;
        GLuint aTexCoordAttribute = 0;
        GLuint uTextureuniform = 0;

        int mPreviewWidth = 0;
        int mPreviewHeight = 0;

        GLuint mOESTexture = 0;

        EglCore * mEglInstance = NULL;

        GLProgram * mCameraProgram = NULL;

        static std::string VertexShared();
        static std::string FragmentShared();

        void newFrameReadyAtTime();

    public:
        jobject javaCamera = NULL;

        Camera();
        ~Camera();

        EglCore * getEglInstance();

        void onSurfaceChanged();

        void setPreviewWidth(int previewWidth){
            mPreviewWidth = previewWidth;
        }

        void setPreviewHeight(int previewHeight){
            mPreviewHeight = previewHeight;
        }

        void genSurfaceTexture();
        GLuint getSurfaceTexture();

        void surfaceTextureAvailable();




    };
}


#endif //GPUCAMERAVIDEO_CAMERA_H
