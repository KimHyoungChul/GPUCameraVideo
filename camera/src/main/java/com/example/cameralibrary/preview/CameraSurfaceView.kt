package com.example.cameralibrary.preview

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.example.cameralibrary.camera.Camera

/**
 * Created by liuxuan on 2018/12/26
 */

class CameraSurfaceView : SurfaceView, SurfaceHolder.Callback{

    private val camera: Camera = Camera() //这个context是直接从view类继承而来的
    private var cameraSurfaceTexture: CameraSurfaceTexture? = null

    private var nativeTextureId: Int = 0
    private var nativeOutputSurfaceAddress: Long = 0

    constructor(context : Context) : super(context)

    constructor(context: Context, attributes: AttributeSet) : super(context, attributes)

    init {
        holder.addCallback(this)
    }

    private fun genTexture(textureCallback : (nativeCamera: Long, surfaceTexture: Int) -> Unit){
        nativeTextureId = nativeGenTexture(camera.nativeInputCameraAddress)
        textureCallback(camera.nativeInputCameraAddress, nativeTextureId)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

        if(holder != null) {
            nativeOutputSurfaceAddress = nativeSurfaceWindowInit(holder.surface) //初始化native层egl的surface表面
        }

    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        genTexture { nativeCamera, surfaceTexture ->
            cameraSurfaceTexture = CameraSurfaceTexture(nativeCamera, surfaceTexture).apply {
                setOnFrameAvailableListener{
                    updateTexImage()
                    onSurfaceTextureAvailable(nativeCamera)
                }
            }
        }

        camera.openCamera(cameraSurfaceTexture, width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        camera.stopPreview()
    }

    fun closeCamera(){
        camera.closeCamera()
    }

    private external fun nativeSurfaceWindowInit(surface: Any): Long

    private external fun nativeGenTexture(nativeCameraAddress: Long) : Int

    private external fun onSurfaceTextureAvailable(nativeCameraAddress: Long)
}
