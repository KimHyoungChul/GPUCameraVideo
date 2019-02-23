//
// Created by 刘轩 on 2019/2/5.
//

#include <jni.h>
#include <android/native_window_jni.h>
#include "MediaPlayer.h"

using namespace GCVBase;

extern "C" {

jlong Java_com_example_codeclibrary_MoviePlayer_nativePlayerInit(JNIEnv * env, jobject obj, jstring readPath, jobject surface){

    MediaPlayer *mediaPlayer = nullptr;
    ANativeWindow * nativeWindow = nullptr;

    if(readPath) {
        const char *readString = env->GetStringUTFChars(readPath, nullptr);

        if(surface){
            nativeWindow = ANativeWindow_fromSurface(env, surface);
        }

        mediaPlayer = new MediaPlayer(readString, nativeWindow);
        env->ReleaseStringChars(readPath, reinterpret_cast<const jchar *>(readString));
    } else{
        mediaPlayer = new MediaPlayer();
    }

    return (jlong)mediaPlayer;
}

void Java_com_example_codeclibrary_MoviePlayer_nativeSetFilePath(JNIEnv * env, jobject obj, jlong nativePlayerAddress, jstring readPath){
    if(!nativePlayerAddress){
        return;
    }

    if(!readPath){
        return;
    }

    const char *readString = env->GetStringUTFChars(readPath, nullptr);

    MediaPlayer * mediaPlayer = (MediaPlayer * )nativePlayerAddress;
    mediaPlayer->setFilePath(readString);

    env->ReleaseStringChars(readPath, reinterpret_cast<const jchar *>(readString));
}

void Java_com_example_codeclibrary_MoviePlayer_nativeStartPlayer(JNIEnv * env, jobject obj, jlong nativePlayerAddress){
    if(!nativePlayerAddress){
        return;
    }

    MediaPlayer * mediaPlayer = (MediaPlayer * )nativePlayerAddress;
    mediaPlayer -> startPlayer([=]{

    });
}

void Java_com_example_codeclibrary_MoviePlayer_nativePausePlayer(JNIEnv * env, jobject obj, jlong nativePlayerAddress){
    if(!nativePlayerAddress){
        return;
    }

    MediaPlayer * mediaPlayer = (MediaPlayer * )nativePlayerAddress;
    mediaPlayer ->pausePlayer([=]{

    });
}

void Java_com_example_codeclibrary_MoviePlayer_nativeCancelPalyer(JNIEnv * env, jobject obj, jlong nativePlayerAddress){
    if(!nativePlayerAddress){
        return;
    }

    MediaPlayer * mediaPlayer = (MediaPlayer * )nativePlayerAddress;
    mediaPlayer ->cancelPlayer([=]{

    });
}

void Java_com_example_codeclibrary_MoviePlayer_nativeSetPlayerSurface(JNIEnv * env, jobject obj, jlong nativePlayerAddress, jobject surface){
    ANativeWindow * nativeWindow = ANativeWindow_fromSurface(env, surface);
    MediaPlayer * mediaPlayer = (MediaPlayer * )nativePlayerAddress;
    mediaPlayer->setNativeSurface(nativeWindow);
}


void Java_com_example_codeclibrary_MoviePlayer_nativeSurfaceDestroyed(JNIEnv * env, jobject obj, jlong nativePlayerAddress){
    MediaPlayer * mediaPlayer = (MediaPlayer * )nativePlayerAddress;
    delete mediaPlayer;
}

}
