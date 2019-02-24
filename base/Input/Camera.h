//
// Created by 刘轩 on 2018/12/27.
//

#ifndef GPUCAMERAVIDEO_CAMERA_H
#define GPUCAMERAVIDEO_CAMERA_H

#include "Rotation.hpp"
#include "Time.hpp"
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

        FacingMode mFacingMode;
        RotationMode rotationMode;
        Rotation rotationCamera = Rotation::defaultRotation();

        GLuint mOESTexture = 0;

        EglCore * mEglInstance = NULL;

        GLProgram * mCameraProgram = NULL;

        static std::string VertexShared();
        static std::string FragmentShared();

        void newFrameReadyAtTime();

    public:

        Camera(int mFacing);
        ~Camera();

        EglCore * getEglInstance();

        void onSurfaceChanged();
        void facingChanged(int facing);
        void orientationChanged(int orientation);

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
