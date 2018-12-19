//
// Created by 刘轩 on 2018/12/17.
//
#include <jni.h>
#include <string>
#include "GLContext.hpp"

extern "C"
JNIEXPORT jstring JNICALL Java_com_example_codeclibrary_CodecMain_stringFromJNI(JNIEnv *env, jobject /* this */) {
    GLContext* context = new GLContext();
//    std::string hello = context->hello;
    context->setString();
    std::string hello = context->hello;
    return env->NewStringUTF(hello.c_str());
}

