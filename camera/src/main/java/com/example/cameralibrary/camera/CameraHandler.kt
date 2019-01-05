package com.example.cameralibrary.camera

import android.os.Handler
import android.os.HandlerThread

/**
 * Created by liuxuan on 2018/12/27
 */
class CameraHandler(private val handlerThread: HandlerThread): Handler(handlerThread.looper) {

    companion object {
        /**
         * 重载运算符，重载invoke函数，相当于重载CameraHandler()，也就是创建一个CameraHandler实例
         */
        operator fun invoke(): CameraHandler{
            val handlerThread = HandlerThread("CameraImpl")
            handlerThread.start()
            return CameraHandler(handlerThread)
        }
    }

    fun postRunnable(runnable: Runnable){
        post {
            runnable.run()
        }
    }

}