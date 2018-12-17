//
// Created by 刘轩 on 2018/12/17.
//
#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_example_codeclibrary_CodecMain_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello Codec";
    return env->NewStringUTF(hello.c_str());
}

