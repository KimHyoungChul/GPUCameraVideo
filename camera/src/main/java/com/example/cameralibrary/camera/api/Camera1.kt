package com.example.cameralibrary.camera.api

import android.graphics.SurfaceTexture
import android.hardware.Camera
import com.example.cameralibrary.camera.CameraAttributes
import com.example.cameralibrary.camera.CameraSize
import java.nio.ByteBuffer
import java.util.jar.Attributes

/**
 * Created by liuxuan on 2018/12/27
 */
class Camera1 : CameraApi() {

    private var camera: Camera? = null


    override fun openCamera(callback: CameraOpenCallback) {
        handler{ //handler函数只有一个runnable参数，因此直接用lamada表达式写在括号外面
            val currentCamera = camera
            if(currentCamera != null){
                callback.onError()
                return@handler
            }

            try {
                val cameraInfo = Camera.CameraInfo()
                val cameraId = (0 until Camera.getNumberOfCameras()).find {
                    Camera.getCameraInfo(it, cameraInfo)
                    cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_BACK //相机朝向的方向
                }

                if(cameraId == null) {
                    callback.onError()
                    return@handler
                }

                val camera = Camera.open(cameraId)
                camera.setErrorCallback{ error, _ ->
                    when(error){
                        Camera.CAMERA_ERROR_EVICTED -> RuntimeException("CAMERA_ERROR_EVICTED")
                        Camera.CAMERA_ERROR_SERVER_DIED -> RuntimeException("CAMERA_ERROR_SERVER_DIED")
                        Camera.CAMERA_ERROR_UNKNOWN -> RuntimeException("CAMERA_ERROR_UNKNOWN")
                        else -> RuntimeException("Undefined camera error received, but assuming fatal.")
                    }
                }

                this.camera = camera
                callback.onOpen(CameraAttributes(cameraInfo, camera.parameters))

            } catch (e: Exception) {
                callback.onError()
            }
        }
    }

    override fun closeCamera(callback: CameraCloseCallback) {

        handler {
            val currentCamera = camera
            if(currentCamera == null){
                callback.onError()
                return@handler
            }

            try {
                currentCamera.release()
            }catch (e: Exception){
                callback.onError()
            }finally {
                camera = null
                callback.onClose()
            }
        }
    }

    override fun startPreview(surfaceTexture: SurfaceTexture, previewSize: CameraSize,
                              displayOrientation: Int, callback: PreViewStartCallback) {
        handler {
            val currentCamera = camera
            if(currentCamera == null){
                callback.onError()
                return@handler
            }

            try{
                val parameters = currentCamera.parameters
                parameters.setPreviewSize(previewSize.width, previewSize.height)
                currentCamera.parameters = parameters
                currentCamera.setDisplayOrientation(displayOrientation)
                currentCamera.setPreviewTexture(surfaceTexture)
                //使用此方法注册预览回调接口时，会将下一帧数据回调给onPreviewFrame()方法，之后这个接口将被销毁。也就是只会回调一次预览帧数据。
                currentCamera.setOneShotPreviewCallback{ _, _ ->
                    callback.onStart()
                }
                //一旦使用此方法注册预览回调接口，onPreviewFrame()方法会一直被调用，直到camera preview销毁
                currentCamera.setPreviewCallback{ bytes: ByteArray, _ ->
                    callback.onPreviewFrame(ByteBuffer.wrap(bytes))
                }
                currentCamera.startPreview()

            }catch (e: Exception){
                callback.onError()
            }
        }
    }

    override fun stopPreview(callback: PreviewStopCallback) {
        handler{
            val currentCamera = camera
            if(currentCamera == null){
                callback.onError()
                return@handler
            }

            try {
                currentCamera.stopPreview()
            }catch (e: Exception){
                callback.onError()
            }finally {
                callback.onStop()
            }
        }
    }


}