//
// Created by 刘轩 on 2018/12/27.
//
#include <jni.h>
#include <android/native_window_jni.h>
#include "Context.h"

using namespace GCVBase;

extern "C" {

jlong Java_com_example_cameralibrary_preview_CameraSurfaceView_nativeSurfaceWindowInit(JNIEnv *env, jobject obj, jobject surface){

    Context::initSharedContext(env);
    ANativeWindow * nativeWindow = ANativeWindow_fromSurface(env, surface);
    Context::getShareContext()->setNativeWindow(nativeWindow);
    return 0;
}

}

