package com.example.cameralibrary.camera.api

import android.graphics.SurfaceTexture
import android.hardware.Camera
import com.example.cameralibrary.camera.AspectRatio
import com.example.cameralibrary.camera.CameraHandler
import com.example.cameralibrary.camera.CameraSize
import com.example.cameralibrary.preview.PreviewImpl
import java.nio.ByteBuffer

/**
 * Created by liuxuan on 2018/12/27
 */
abstract class CameraImpl {

    private val cameraHandler : CameraHandler = CameraHandler()

    protected var mPreview: PreviewImpl? = null

    protected fun handler(runnable: (()->Unit)){ //()表示无参函数，->Unit表示返回类型为Unit（也就是没有返回值类型），整体(()->Unit)表示参数runnable
        cameraHandler.postRunnable(Runnable(runnable)) //这里构造了一个Runnable对象，Runnable()参数类型本身就要求为()->Unit
    }

    /**
     * 相机生命周期函数
     */
    abstract fun openCamera(mFacing: Int, callback: CameraOpenCallback)
    abstract fun closeCamera(callback: CameraCloseCallback)
    abstract fun startPreview(surfaceTexture: SurfaceTexture, callback: PreviewStartCallback)
    abstract fun stopPreview(callback: PreviewStopCallback)


    /**
     * 相机生命周期回调
     */
    interface CameraOpenCallback {
        fun onOpen(mCamera:Camera)
        fun onError()
    }

    interface CameraCloseCallback {
        fun onClose()
        fun onError()
    }

    interface PreviewStartCallback {
        fun onStart()
        fun onPreviewFrame(previewData:ByteBuffer)
        fun onError()
    }

    interface PreviewStopCallback {
        fun onStop()
        fun onError()
    }


    /**
     * 设置相机所在的预览界面的引用
     */
    abstract fun setPreviewImpl(mPreview: PreviewImpl)


    /**
     * 设置相机参数
     */
    abstract fun setFacing(facing: Int)
    abstract fun setFlash(flash: Int)
    abstract fun setAutoFocus(autoFocus: Boolean)
    abstract fun setAspectRatio(ratio: AspectRatio): Boolean
    abstract fun getSupportedAspectRatios(): Set<AspectRatio>
    abstract fun setDisplayOrientation(displayOrientation: Int)


    /**
     * 拍照
     */
    abstract fun takePicture()
}