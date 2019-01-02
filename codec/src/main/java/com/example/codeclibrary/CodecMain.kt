package com.example.codeclibrary

/**
 * Created by liuxuan on 2018/12/17
 */
class CodecMain {

    fun getStringFromCodec(): String{
        return stringFromJNI()
    }

    private external fun stringFromJNI(): String //java中用native关键字

}