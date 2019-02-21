package com.example.filterlibrary

/**
 * Created by liuxuan on 2019/1/27
 */
abstract class Filter constructor( width: Int = 0, height: Int = 0) {

    private var nativeFilterAdress: Long = 0L

    init {
        initFilter(width, height)
    }

    private fun initFilter(width: Int = 0, height: Int = 0){
        nativeFilterAdress = nativeFilterInit(getVertexShader(), getFragmentShader(), width, height)
    }

    fun getNativeFilterAdress(): Long{
        return nativeFilterAdress
    }

    abstract fun getVertexShader(): String
    abstract fun getFragmentShader(): String

    private external fun nativeFilterInit(vertexShader: String, fragmentShader: String, width: Int, height: Int): Long
}