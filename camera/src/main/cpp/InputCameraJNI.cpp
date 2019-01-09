//
// Created by 刘轩 on 2018/12/18.
//

#include <jni.h>
#include "camera.h"

using namespace GCVBase;

extern "C" {

jlong Java_com_example_cameralibrary_camera_Camera_nativeCameraInit(JNIEnv *env, jobject obj) {
    Camera * camera = new Camera(); // TODO 需要用智能指针替换之
    camera->javaCamera = env->NewGlobalRef(obj);
    return (jlong) camera;
}

}

