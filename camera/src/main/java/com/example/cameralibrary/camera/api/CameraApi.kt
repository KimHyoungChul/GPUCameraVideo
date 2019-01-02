package com.example.cameralibrary.camera.api

import android.graphics.SurfaceTexture
import com.example.cameralibrary.camera.CameraAttributes
import com.example.cameralibrary.camera.CameraHandler
import com.example.cameralibrary.camera.CameraSize
import java.nio.ByteBuffer

/**
 * Created by liuxuan on 2018/12/27
 */
abstract class CameraApi {

    private val cameraHandler : CameraHandler = CameraHandler()

    protected fun handler(runnable: (()->Unit)){ //()表示无参函数，->Unit表示返回类型为Unit（也就是没有返回值类型），整体(()->Unit)表示参数runnable
        cameraHandler.postRunnable(Runnable(runnable)) //这里构造了一个Runnable对象，Runnable()参数类型本身就要求为()->Unit
    }


    abstract fun openCamera(callback: CameraOpenCallback)
    abstract fun closeCamera(callback: CameraCloseCallback)
    abstract fun startPreview(surfaceTexture: SurfaceTexture, previewSize: CameraSize,
                              displayOrientation: Int, callback: PreViewStartCallback)
    abstract fun stopPreview(callback: PreviewStopCallback)

    interface CameraOpenCallback {
        fun onOpen(cameraAttributes: CameraAttributes)
        fun onError()
    }

    interface CameraCloseCallback {
        fun onClose()
        fun onError()
    }

    interface PreViewStartCallback {
        fun onStart()
        fun onPreviewFrame(previewData:ByteBuffer)
        fun onError()
    }

    interface PreviewStopCallback {
        fun onStop()
        fun onError()
    }
}