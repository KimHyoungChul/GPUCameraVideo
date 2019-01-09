//
// Created by 刘轩 on 2019/1/8.
//
#include <jni.h>
#include <NativeOutput.h>
#include <NativeInput.h>

using namespace GCVBase;

extern "C" {

void Java_com_example_baselib_GCVInput_nativeAddTarget(JNIEnv *env, jobject obj,
                                                                      jlong inputAddress, jlong outputAddress) {
    if(inputAddress == NULL){
        // TODO 报错
        return;
    }

    NativeInput * input = (NativeInput *)inputAddress;
    NativeOutput * output = (NativeOutput *) outputAddress;

    input->addTarget(output);
}

void Java_com_example_baselib_GCVInput_nativeRemoveTarget(JNIEnv *env, jobject obj,
                                                                      jlong inputAddress, jlong outputAddress) {
    if(inputAddress == NULL){
        // TODO 报错
        return;
    }

    NativeInput * input = (NativeInput *)inputAddress;
    NativeOutput * output = (NativeOutput *) outputAddress;

    input->removeTarget(output);
}

}

