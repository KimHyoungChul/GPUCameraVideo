//
// Created by 刘轩 on 2019/2/7.
//

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "DisplayView.h"

using namespace GCVBase;

extern "C" {

jlong Java_com_example_codeclibrary_playerview_SurfacePlayerview_nativeSurfaceWindowInit(JNIEnv *env, jobject obj, jobject surface){
    Context::initSharedContext(env);
    ANativeWindow * nativeWindow = ANativeWindow_fromSurface(env, surface);
    DisplayView * displayView = nullptr;

    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [&displayView, &nativeWindow]{
        Context::getShareContext()->setNativeWindow(nativeWindow);
        displayView = new DisplayView(ANativeWindow_getWidth(nativeWindow), ANativeWindow_getHeight(nativeWindow));
    });

    return (jlong)displayView;
}

void Java_com_example_codeclibrary_playerview_SurfacePlayerview_nativeSurfaceWindowDestroyed(JNIEnv *env, jobject obj, jlong nativeOutputSurfaceAddress){
    auto * displayView = (DisplayView * )nativeOutputSurfaceAddress;
    delete displayView;
}

}