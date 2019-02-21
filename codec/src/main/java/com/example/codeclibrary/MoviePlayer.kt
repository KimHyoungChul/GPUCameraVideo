package com.example.codeclibrary

import android.view.Surface
import com.example.baselib.GCVInput

/**
 * Created by liuxuan on 2019/2/5
 */
class MoviePlayer(readPath: String? = null, surface: Surface? = null) : GCVInput(){

    init {
        inputObjectAdress = nativePlayerInit(readPath, surface)
    }

    fun setFilePath(readPath: String){
        nativeSetFilePath(inputObjectAdress, readPath)
    }

    fun startPlayer(){
        nativeStartPlayer(inputObjectAdress)
    }

    fun pausePlayer(){
        nativePausePlayer(inputObjectAdress)
    }

    fun cancelPalyer(){
        nativeCancelPalyer(inputObjectAdress)
    }


    private external fun nativePlayerInit(readPath: String?, surface: Surface?): Long

    private external fun nativeStartPlayer(inputObjectAdress: Long)

    private external fun nativePausePlayer(inputObjectAdress: Long)

    private external fun nativeCancelPalyer(inputObjectAdress: Long)

    private external fun nativeSetFilePath(inputObjectAdress: Long, readPath: String)

}