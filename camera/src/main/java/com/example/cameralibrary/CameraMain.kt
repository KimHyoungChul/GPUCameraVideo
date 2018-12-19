package com.example.cameralibrary

/**
 * Created by liuxuan on 2018/12/18
 * Email : lx13@meitu.com
 */
class CameraMain {
    fun getStringFromCameraCodec(): String{
        return stringFromCameraJNI()
    }

    private external fun stringFromCameraJNI(): String //java中用native关键字

//    companion object {
//
//        init {
//            System.loadLibrary("codec-lib")
//        }
//    }
}