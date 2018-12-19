//
// Created by 刘轩 on 2018/12/18.
//

#include <jni.h>
#include <string>
#include "FilterContext.hpp"

extern "C"
JNIEXPORT jstring JNICALL Java_com_example_cameralibrary_CameraMain_stringFromCameraJNI(JNIEnv *env, jobject /* this */) {
    FilterContext* context = new FilterContext();
    context->setString();
    std::string hello = context->hello;
    return env->NewStringUTF(hello.c_str());
}

