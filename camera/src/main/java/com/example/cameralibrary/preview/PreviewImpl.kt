package com.example.cameralibrary.preview

import android.content.Context
import android.graphics.SurfaceTexture
import android.view.View
import com.example.baselib.GCVInput
import com.example.baselib.GCVOutput
import com.example.cameralibrary.camera.Camera
import java.nio.ByteBuffer

/**
 * Created by liuxuan on 2019/1/2
 */

abstract class PreviewImpl(context: Context) {

    private var mWidth: Int = 0
    private var mHeight: Int = 0

    /*
     * 可以看到，这里我们强制使SurfaceView或者TextureView这些预览控件持有Camera的引用，这样设计的初衷是为了在
     * SurfaceView的生命周期中管理Camera的生命周期，而SurfaceView的生命周期则有activity或者fragment来管理
     */
    protected val mCamera: Camera = Camera(context) // TODO 这里应该预留参数，让用户可以选择到底用Canera1还是Camera2

    abstract fun getView(): View?

    abstract fun setRecorder(movieRecorder : GCVOutput)
    abstract fun setFacing(facing: Int)
    abstract fun getFacing(): Int

    abstract fun takePicture()

    abstract fun previewIsReady(): Boolean

    abstract fun setPreviewLifeListener(previewLife: Preview.PreviewLifeListener)
    abstract fun setPreviewDataListener(previewData: Preview.PreviewDataListener)

    abstract fun setFilterGroup(filterGroup: GCVInput)

    fun setPreviewSize(width: Int, height: Int) {
        mWidth = width
        mHeight = height
    }

    fun getWidth(): Int {
        return mWidth
    }

    fun getHeight(): Int {
        return mHeight
    }

    interface SurfaceListener {
        fun onSurfaceCreated(nativeOutputAddress: Long)
        fun onSurfaceChanged(surfaceTexture: SurfaceTexture?, width: Int, height: Int)
        fun onSurfaceDestory()
    }

    interface CameraOpenListener {
        fun onCameraOpen()
        fun onOpenError()
    }

    interface CameraPreviewListener {
        fun onPreviewStart()
        fun onPreviewFrame(previewFrameData: ByteBuffer)
        fun onPreviewError()
    }

    interface CameraTakePictureListener {
        fun onPictureTaken(cameraPictureData: ByteBuffer)
        fun onPictureError()
    }
}