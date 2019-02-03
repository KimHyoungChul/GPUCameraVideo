//
// Created by 刘轩 on 2018/12/18.
//

#include <jni.h>
#include "camera.h"

using namespace GCVBase;

extern "C" {

jlong Java_com_example_cameralibrary_camera_Camera_nativeCameraInit(JNIEnv *env, jobject obj, jint mFacing) {
    Camera * camera = new Camera(mFacing); // TODO 需要用智能指针替换之
    return (jlong) camera;
}

void Java_com_example_cameralibrary_camera_Camera_nativeChangeCameraFacing(JNIEnv *env, jobject obj, jlong nativeCamera, jint mFacing) {
    Camera * camera = (Camera *) nativeCamera;
    camera->facingChanged(mFacing);
}

void Java_com_example_cameralibrary_camera_Camera_nativeChangeCamerOrientation(JNIEnv *env, jobject obj, jlong nativeCamera, jint orientation) {
    Camera * camera = (Camera *) nativeCamera;
    camera->orientationChanged(orientation);
}

}

