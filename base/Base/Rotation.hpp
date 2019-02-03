//
// Created by 刘轩 on 2019/2/3.
//

#ifndef GPUCAMERAVIDEO_ROTATION_H
#define GPUCAMERAVIDEO_ROTATION_H

#include <GLES2/gl2.h>

namespace GCVBase {
    typedef enum {
        rotation0,
        rotation90,
        rotation270,
    }RotationMode;

    typedef enum {
        FacingBack,
        FacingFront,
    }FacingMode;


    union Rotation {
        struct{
            RotationMode rotationMode;
            FacingMode facing;
        };

        static const Rotation & defaultRotation(){
            static const Rotation rotation(RotationMode::rotation90, FacingMode::FacingBack);
            return rotation;
        }

        Rotation(RotationMode mRotationMode, FacingMode mFacing){
            rotationMode = mRotationMode;
            facing = mFacing;
        }
    };

    typedef Rotation rotation;


    static const GLfloat * CalculateDisplayRotation(const Rotation &rotation){

        if(rotation.facing == FacingMode::FacingBack){
            switch(rotation.rotationMode){
                case RotationMode::rotation0 : //注意这里的旋转0°是指屏幕竖直放置
                    static const GLfloat texCoord0[] = { //预览的纹理需要先倒置，再进行右旋90°
                            1.0f, 1.0f,
                            1.0f, 0.0f,
                            0.0f, 1.0f,
                            0.0f, 0.0f,
                    };
                    return texCoord0;
                case RotationMode::rotation90: //注意这里的旋转90°是指屏幕逆时针旋转90°，也就是向左旋转90°
                    static const GLfloat texCoord90[] = { //预览的纹理不需要倒置，直接上下颠倒
                            0.0f, 1.0f,
                            1.0f, 1.0f,
                            0.0f, 0.0f,
                            1.0f, 0.0f,
                    };
                    return texCoord90;

                case RotationMode::rotation270: //注意这里的旋转270°是指屏幕逆时针旋转270°，也就是向右旋转90°
                    static const GLfloat texCoord270[] = { //预览的纹理不需要倒置，直接左右反转
                            1.0f, 0.0f,
                            0.0f, 0.0f,
                            1.0f, 1.0f,
                            0.0f, 1.0f,
                    };
                    return texCoord270;
            }
        }
    }

    static const GLfloat * CalculateRecoderRotation(const Rotation &rotation){
        if(rotation.facing == FacingMode::FacingBack){
            switch(rotation.rotationMode){
                case RotationMode::rotation0 :
                    static const GLfloat texCoord0[] = { //录制的纹理不需要倒置，直接对原坐标进行右旋90°即可
                             0.0f, 1.0f,
                             0.0f, 0.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f,
                    };
                    return texCoord0;
                case RotationMode::rotation90:
                    static const GLfloat texCoord90[] = { //录制的纹理不需要倒置，直接用原坐标即可
                            0.0f, 0.0f,
                            1.0f, 0.0f,
                            0.0f, 1.0f,
                            1.0f, 1.0f,
                    };
                    return texCoord90;
                case RotationMode::rotation270:
                    static const GLfloat texCoord270[] = { //录制的纹理不需要倒置，直接对原坐标进行右旋180°即可
                            1.0f, 1.0f,
                            0.0f, 1.0f,
                            1.0f, 0.0f,
                            0.0f, 0.0f,
                    };
                    return texCoord270;
            }
        }
    }
}

#endif //GPUCAMERAVIDEO_ROTATION_H
