package com.example.cameralibrary

/**
 * Created by liuxuan on 2018/12/18
 */
class CameraMain {
    fun getStringFromCameraCodec(): String{
        return stringFromCameraJNI()
    }

    private external fun stringFromCameraJNI(): String //java中用native关键字

}