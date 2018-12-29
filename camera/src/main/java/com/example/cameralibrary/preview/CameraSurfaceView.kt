package com.example.cameralibrary.preview

import android.content.Context
import android.util.AttributeSet
import android.util.Log
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.example.cameralibrary.camera.Camera
import com.example.cameralibrary.camera.CameraAttributes
import com.example.cameralibrary.camera.CameraSize
import com.example.cameralibrary.camera.CameraSizeCalculator

/**
 * Created by liuxuan on 2018/12/26
 */

class CameraSurfaceView : SurfaceView, SurfaceHolder.Callback{

    private val camera: Camera = Camera(context) //这个context是直接从view类继承而来的
    private var cameraSurfaceTexture: CameraSurfaceTexture? = null
    private var cameraAttributes: CameraAttributes? = null

    private var state: CameraState = CameraState.RELEASED
    private var displayOrientation: Int = 0
    private var nativeTextureId: Int = 0

    private var nativeInputCameraAddress: Long = 0
    private var nativeOutputSurfaceAddress: Long = 0

    constructor(context : Context) : super(context)

    constructor(context: Context, attributes: AttributeSet) : super(context, attributes)

    init {
        holder.addCallback(this)
    }

    private fun genTexture(textureCallback : (nativeCamera: Long, surfaceTexture: Int) -> Unit){
        nativeTextureId = nativeGenTexture(nativeInputCameraAddress)
        textureCallback(nativeInputCameraAddress, nativeTextureId)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

        if(holder != null) {
            nativeOutputSurfaceAddress = nativeSurfaceWindowInit(holder.surface) //初始化native层egl的surface表面
        }
        nativeInputCameraAddress = nativeCameraInit()
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

        if (state == CameraState.RELEASED) {
            open()
        }
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {

    }


    fun open() {
        camera.openCamera(object : Camera.onCameraOpened{ // 异步线程回调之后才能开始startPreview
            override fun onOpened(attributes: CameraAttributes) {
                start(attributes)
            }

            override fun onOpenedError() {
            }
        })
        state = CameraState.OPENED
    }

    fun start(attributes: CameraAttributes) {
        cameraAttributes = attributes
        val surfaceTexture = cameraSurfaceTexture
        if (surfaceTexture != null) {
            val previewOrientation = (attributes.sensorOrientation - displayOrientation + 360) % 360
            val previewSize = CameraSizeCalculator(attributes.previewSizes)
                    .findClosestSizeContainingTarget(when (previewOrientation % 180 == 0) {
                        true -> CameraSize(width, height)
                        false -> CameraSize(height, width)
                    })

            try {
                camera.startPreview(surfaceTexture, previewSize, previewOrientation)
            } catch (e: Exception) {

            }
        }

        state = CameraState.STARTED
    }

    fun stop() {

        try {
            camera.stopPreview()
        } catch (e: Exception) {
        }

        state = CameraState.STOPPED
    }


    fun release() {
        try {
            camera.closeCamera()
        } catch (e: Exception) {
        }

        state = CameraState.RELEASED
    }



    private enum class CameraState {
        OPENED,
        STARTED,
        STOPPED,
        RELEASED;
    }

    private external fun nativeSurfaceWindowInit(surface: Any): Long

    private external fun nativeCameraInit(): Long

    private external fun nativeGenTexture(nativeCameraAddress: Long) : Int

    private external fun onSurfaceTextureAvailable(nativeCameraAddress: Long)


}
