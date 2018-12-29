package com.example.cameralibrary.camera

import android.content.Context
import android.graphics.SurfaceTexture
import android.view.Surface
import com.example.cameralibrary.camera.api.Camera1
import com.example.cameralibrary.camera.api.CameraApi
import com.example.cameralibrary.preview.CameraSurfaceTexture
import java.nio.ByteBuffer

/**
 * Created by liuxuan on 2018/12/27
 */
class Camera(context : Context)  {

    private val cameraApi: CameraApi

    init {
        cameraApi = Camera1() //暂时不考虑Camera2接口，但保留
    }


    fun openCamera(cameraOpenedCall: onCameraOpened) {
        cameraApi.openCamera(object : CameraApi.CameraOpenCallback{
            override fun onOpen(cameraAttributes: CameraAttributes) {
                cameraOpenedCall.onOpened(cameraAttributes)
            }

            override fun onError() {
                // TODO "添加报错 log"
            }
        })

    }

    fun startPreview(cameraSurfaceTexture: CameraSurfaceTexture, previewSize: CameraSize, previewOrientation: Int){
        cameraSurfaceTexture.setDefaultBufferSize(previewSize.width, previewSize.height)
        cameraSurfaceTexture.size = when(previewOrientation % 180) {
            0 -> previewSize
            else -> CameraSize(previewSize.height, previewSize.width)
        }

        cameraApi.startPreview(cameraSurfaceTexture, previewSize, previewOrientation, object : CameraApi.PreViewStartCallback {
            override fun onStart() {

            }

            override fun onPreviewFrame(previewData: ByteBuffer) {
                // TODO 再做一层接口，将previewFrame数据返回上去，可以做很多事情
            }

            override fun onError() {

            }

        })
    }

    fun stopPreview(){
        cameraApi.stopPreview(object : CameraApi.PreviewStopCallback{
            override fun onStop() {

            }

            override fun onError() {

            }

        })
    }

    fun closeCamera() {
        cameraApi.closeCamera(object : CameraApi.CameraCloseCallback {
            override fun onClose() {

            }

            override fun onError() {

            }
        })
    }

    interface onCameraOpened{
        fun onOpened(attributes: CameraAttributes)
        fun onOpenedError()
    }

}