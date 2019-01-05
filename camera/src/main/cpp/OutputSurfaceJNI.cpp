//
// Created by 刘轩 on 2018/12/27.
//
#include <jni.h>
#include <android/native_window_jni.h>
#include "Context.h"
#include "camera.h"

using namespace GCVBase;

extern "C" {

jlong Java_com_example_cameralibrary_preview_surfaceview_SurfaceCallback_nativeSurfaceWindowInit(JNIEnv *env, jobject obj, jobject surface){

    Context::initSharedContext(env);
    ANativeWindow * nativeWindow = ANativeWindow_fromSurface(env, surface);
    Context::getShareContext()->setNativeWindow(nativeWindow);
    return 0;
}

jint Java_com_example_cameralibrary_preview_surfaceview_SurfaceCallback_nativeGenTexture(JNIEnv *env, jobject obj, jlong nativeCamera) {
    if(!nativeCamera){
        return 0;
    }

    Camera * camera = (Camera *)nativeCamera;
    camera -> genSurfaceTexture();
    return camera -> getSurfaceTexture();
}

void Java_com_example_cameralibrary_preview_surfaceview_SurfaceCallback_onSurfaceTextureAvailable(JNIEnv *env, jobject obj, jlong nativeCamera) {
    if (!nativeCamera) {
        return;
    }
    Camera * camera = (Camera *)nativeCamera;
    if(!camera->getEglInstance()->isCurrentContext()){
        camera->getEglInstance()->makeAsCurrent();
    }

}

void Java_com_example_cameralibrary_preview_surfaceview_SurfaceCallback_surfaceTextureAvailable(JNIEnv *env, jobject obj, jlong nativeCamera) {
    if(!nativeCamera){
        return ;
    }

    Camera * camera = (Camera *)nativeCamera;
    camera -> surfaceTextureAvailable();
}

void Java_com_example_cameralibrary_preview_surfaceview_SurfaceCallback_nativeOnSurfaceChanged(JNIEnv *env, jobject obj, jlong nativeCamera, jint width, jint height) {
    if(!nativeCamera){
        return ;
    }

    Camera * camera = (Camera *)nativeCamera;
    camera ->setPreviewWidth(width);
    camera ->setPreviewHeight(height);
}

}

