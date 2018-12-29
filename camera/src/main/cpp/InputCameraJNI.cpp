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

jlong Java_com_example_cameralibrary_camera_Camera_nativeCameraInit(JNIEnv *env, jobject obj) {
    Camera * camera = new Camera();
    camera->javaCamera = env->NewGlobalRef(obj);
    return (jlong) camera;
}




}

