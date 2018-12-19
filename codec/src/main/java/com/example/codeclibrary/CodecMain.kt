package com.example.codeclibrary

/**
 * Created by liuxuan on 2018/12/17
 * Email : lx13@meitu.com
 */
class CodecMain {

    fun getStringFromCodec(): String{
        return stringFromJNI()
    }

    private external fun stringFromJNI(): String //java中用native关键字

//    companion object {
//
//        init {
//            System.loadLibrary("codec-lib")
//        }
//    }
}