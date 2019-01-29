//
// Created by 刘轩 on 2019/1/27.
//

#include <jni.h>
#include <Filter.h>

using namespace GCVBase;

extern "C" {

jlong Java_com_example_filterlibrary_Filter_nativeFilterInit(JNIEnv * env, jobject thiz,
                                                             jstring vertexShader, jstring fragmentShader,
                                                             jint width, jint height){
    const char * vertex = env->GetStringUTFChars(vertexShader, NULL);
    const char * fragment = env->GetStringUTFChars(fragmentShader, NULL);

    Filter * filter;

    if(vertex == NULL && fragment == NULL){
        filter = new Filter(width, height);
    } else{
        filter = new Filter(vertex, fragment, width, height);
    }

    return (jlong)filter;
}

}

