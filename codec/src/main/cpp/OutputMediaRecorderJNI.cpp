//
// Created by 刘轩 on 2018/12/17.
//
#include <jni.h>
#include "EncoderConfig.hpp"
#include "MediaRecorder.h"

using namespace GCVBase;

extern "C" {

jlong Java_com_example_codeclibrary_MovieRecorder_nativeRecorderInit(JNIEnv * env,
                                                                     jobject obj,
                                                                     jstring savePath,
                                                                     jint width,
                                                                     jint height,
                                                                     jlong bitRate,
                                                                     jint outputOrientation,
                                                                     jint mediacodecColorFormat,
                                                                     jint bitRateMode){

    EncoderConfig config;
    config.outputPath = env->GetStringUTFChars(savePath, nullptr);
    config.setOutputSize(Size(width, height));
    config.videoBitRate = (unsigned long) bitRate;
    config.outputOrientation = outputOrientation;
    config.colorYUVFormat = mediacodecColorFormat;
    config.bitRateMode = bitRateMode;

    auto * mediaRecord = new MediaRecorder(config, env);

    return (jlong)mediaRecord;
}

void Java_com_example_codeclibrary_MovieRecorder_nativeStartRecorder(JNIEnv * env, jobject obj, jlong mMediaRecoderAddress){
    if(!mMediaRecoderAddress){
        return;
    }

    auto * mediaRecord = (MediaRecorder*) mMediaRecoderAddress;
    mediaRecord->startRecording([=]{

    });
}

void Java_com_example_codeclibrary_MovieRecorder_nativePauseRecorder(JNIEnv * env, jobject obj, jlong mMediaRecoderAddress){
    if(!mMediaRecoderAddress){
        return;
    }

    auto * mediaRecord = (MediaRecorder*) mMediaRecoderAddress;
    mediaRecord->pauseRecording([=]{

    });
}

void Java_com_example_codeclibrary_MovieRecorder_nativeFinishRecorder(JNIEnv * env, jobject obj, jlong mMediaRecoderAddress){
    if(!mMediaRecoderAddress){
        return;
    }

    auto * mediaRecord = (MediaRecorder*) mMediaRecoderAddress;
    mediaRecord->finishRecording([=]{

    });
}

void Java_com_example_codeclibrary_MovieRecorder_nativecancelRecording(JNIEnv * env, jobject obj, jlong mMediaRecoderAddress){
    if(!mMediaRecoderAddress){
        return;
    }

    auto * mediaRecord = (MediaRecorder*) mMediaRecoderAddress;
    mediaRecord->cancelRecording([=]{

    });
}

void Java_com_example_codeclibrary_MovieRecorder_nativeSurfaceDestory(JNIEnv * env, jobject obj, jlong mMediaRecoderAddress){
    auto * mediaRecord = (MediaRecorder*) mMediaRecoderAddress;
    delete mediaRecord;
}




}

