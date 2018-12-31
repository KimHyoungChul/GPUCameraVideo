package com.example.cameralibrary.camera

import android.util.Log
import com.example.cameralibrary.camera.api.Camera1
import com.example.cameralibrary.camera.api.CameraApi
import com.example.cameralibrary.preview.CameraSurfaceTexture
import java.nio.ByteBuffer

/**
 * Created by liuxuan on 2018/12/27
 */
class Camera {

    private var state: CameraState = CameraState.RELEASED
    private val cameraApi: CameraApi
    private var displayOrientation: Int = 0
    private var nativeInputCameraAddress: Long = 0

    init {
        cameraApi = Camera1() //暂时不考虑Camera2接口，但保留
    }

    fun creatNativeCamera(): Long{
        nativeInputCameraAddress = nativeCameraInit()
        return nativeInputCameraAddress
    }


    fun openCamera(cameraSurfaceTexture: CameraSurfaceTexture?, width: Int, height:Int) {
        cameraApi.openCamera(object : CameraApi.CameraOpenCallback{
            override fun onOpen(cameraAttributes: CameraAttributes) {
                if (cameraSurfaceTexture != null) {
                    val previewOrientation = (cameraAttributes.sensorOrientation - displayOrientation + 360) % 360
                    val previewSize = CameraSizeCalculator(cameraAttributes.previewSizes)
                            .findClosestSizeContainingTarget(when (previewOrientation % 180 == 0) {
                                true -> CameraSize(width, height)
                                false -> CameraSize(height, width)
                            })
                    startPreview(cameraSurfaceTexture, previewSize, previewOrientation)
                }
                state = CameraState.OPENED
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
                state = CameraState.STARTED
            }

            override fun onPreviewFrame(previewData: ByteBuffer) {
                // TODO 再做一层接口，将previewFrame数据返回上去，可以做很多事情
                Log.e("previewData", previewData.long.toString())
            }

            override fun onError() {

            }
        })
    }

    fun stopPreview(){
        cameraApi.stopPreview(object : CameraApi.PreviewStopCallback{
            override fun onStop() {
                state = CameraState.STOPPED
            }

            override fun onError() {

            }

        })
    }

    fun closeCamera() {
        cameraApi.closeCamera(object : CameraApi.CameraCloseCallback {
            override fun onClose() {
                state = CameraState.RELEASED
            }

            override fun onError() {

            }
        })
    }

    private enum class CameraState {
        OPENED,
        STARTED,
        STOPPED,
        RELEASED;
    }

    private external fun nativeCameraInit(): Long

}