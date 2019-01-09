package com.example.baselib

/**
 * Created by liuxuan on 2019/1/7
 */

open class GCVInput {
    protected var inputObjectAdress: Long = 0L

    fun addTarget(output: GCVOutput){
        if(inputObjectAdress == 0L){
            throw Exception("nativeOutput == 0")
        }

        nativeAddTarget(inputObjectAdress, output.nativeOutput())
    }

    fun removeTarget(output: GCVOutput){
        if(inputObjectAdress == 0L){
            throw Exception("nativeOutput == 0")
        }

        nativeRemoveTarget(inputObjectAdress, output.nativeOutput())
    }

    private external fun nativeAddTarget(input: Long, output: Long)
    private external fun nativeRemoveTarget(input: Long, output: Long)
}