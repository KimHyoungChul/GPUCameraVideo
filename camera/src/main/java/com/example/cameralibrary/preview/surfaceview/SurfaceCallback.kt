package com.example.cameralibrary.preview.surfaceview

import android.graphics.SurfaceTexture
import android.view.SurfaceHolder
import com.example.cameralibrary.camera.Camera
import com.example.cameralibrary.preview.PreviewImpl.SurfaceListener

/**
 * Created by liuxuan on 2019/1/2
 */
class SurfaceCallback(camera: Camera, surfaceListener: SurfaceListener) : SurfaceHolder.Callback{

    private val mCamera : Camera = camera

    private var surfaceTexture: SurfaceTexture? = null
    private val mSurfaceChangedListener = surfaceListener

    private var nativeTextureId: Int = 0
    private var nativeOutputSurfaceAddress: Long = 0
    private var nativeInputCameraAddress: Long = 0

    private fun genTexture(textureCallback : (nativeCamera: Long, surfaceTexture: Int) -> Unit){
        nativeInputCameraAddress = mCamera.creatNativeCamera()
        nativeTextureId = nativeGenTexture(nativeInputCameraAddress)

        textureCallback(nativeInputCameraAddress, nativeTextureId)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

        if(holder != null) {
            nativeOutputSurfaceAddress = nativeSurfaceWindowInit(holder.surface) //初始化native层egl的surface表面
        }
        mSurfaceChangedListener.onSurfaceCreated(nativeOutputSurfaceAddress)

        genTexture {
            nativeCamera, surfaceTexture ->
            this.surfaceTexture = SurfaceTexture(surfaceTexture)
            this.surfaceTexture?.setOnFrameAvailableListener{
                onSurfaceTextureAvailable(nativeCamera)
                it.updateTexImage()
                surfaceTextureAvailable(nativeCamera)
            }
        }
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

        nativeOnSurfaceChanged(nativeInputCameraAddress, width, height)

        mSurfaceChangedListener.onSurfaceChanged(surfaceTexture, width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        mSurfaceChangedListener.onSurfaceDestory()
    }



    private external fun onSurfaceTextureAvailable(nativeCameraAddress: Long)

    private external fun surfaceTextureAvailable(nativeCameraAddress: Long)

    private external fun nativeOnSurfaceChanged(nativeCameraAddress: Long, width: Int, height: Int)

    private external fun nativeSurfaceWindowInit(surface: Any): Long

    private external fun nativeGenTexture(nativeCameraAddress: Long) : Int

}