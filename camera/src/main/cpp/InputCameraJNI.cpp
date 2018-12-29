//
// Created by 刘轩 on 2018/12/18.
//

#include <jni.h>
#include <FilterContext.hpp>
#include "camera.h"

using namespace GCVBase;

extern "C" {

jstring Java_com_example_cameralibrary_CameraMain_stringFromCameraJNI(JNIEnv *env, jobject obj) {
    FilterContext *context = new FilterContext();
    context->setString();
    std::string hello = context->hello;
    return env->NewStringUTF(hello.c_str());
}

jlong Java_com_example_cameralibrary_preview_CameraSurfaceView_nativeCameraInit(JNIEnv *env, jobject obj) {
    Camera * camera = new Camera();
    camera->javaCamera = env->NewGlobalRef(obj);
    return (jlong) camera;
}

jint Java_com_example_cameralibrary_preview_CameraSurfaceView_nativeGenTexture(JNIEnv *env, jobject obj, jlong nativeCamera) {
    if(!nativeCamera){
        return 0;
    }

    Camera * camera = (Camera *)nativeCamera;
    camera -> genSurfaceTexture();
    return camera -> getSurfaceTexture();
}

void Java_com_example_cameralibrary_preview_CameraSurfaceView_onSurfaceTextureAvailable(JNIEnv *env, jobject obj, jlong nativeCamera) {
    if(!nativeCamera){
        return ;
    }

    Camera * camera = (Camera *)nativeCamera;
    camera -> surfaceTextureAvailable();
}


}

