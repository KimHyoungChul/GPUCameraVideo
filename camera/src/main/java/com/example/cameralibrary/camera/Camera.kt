package com.example.cameralibrary.camera

import android.content.Context
import android.graphics.SurfaceTexture
import android.hardware.Camera
import android.util.Log
import android.view.WindowManager
import com.example.cameralibrary.camera.api.Camera1
import com.example.cameralibrary.camera.api.CameraImpl
import com.example.cameralibrary.preview.PreviewImpl
import com.example.cameralibrary.preview.PreviewImpl.PreviewCameraOpenListener
import java.nio.ByteBuffer

/**
 * Created by liuxuan on 2018/12/27
 */
class Camera(context: Context) {

    private val cameraImpl: CameraImpl
    private val windowManager: WindowManager
    private var mCamera: Camera? = null

    private var state: CameraState = CameraState.RELEASED

    private var mFacing: Int = 0
    private var mFlash: Int = 0
    private var mAutoFocus: Boolean = false

    init {
        cameraImpl = Camera1() //暂时不考虑Camera2接口，但保留
        windowManager = context.getSystemService(Context.WINDOW_SERVICE) as WindowManager
    }

    private enum class CameraState {
        OPENED,
        STARTED,
        STOPPED,
        RELEASED;
    }

    interface FacingChangedCallback {
        fun onFacingChanged()
    }

    fun isCameraOpened(): Boolean {
        return state == CameraState.OPENED || state == CameraState.STARTED
    }

    fun creatNativeCamera(): Long {
        return nativeCameraInit()
    }

    fun setPreviewImpl(previewImpl: PreviewImpl){
        cameraImpl.setPreviewImpl(previewImpl)
    }


    /************************************** 相机生命周期函数 **********************************************/

    fun openCamera(mFacing: Int, surfaceTexture: SurfaceTexture? = null, previewOpen: PreviewCameraOpenListener? = null) {
        cameraImpl.openCamera(mFacing, object : CameraImpl.CameraOpenCallback {
            override fun onOpen(mCamera:Camera) {
                this@Camera.mCamera = mCamera

                state = CameraState.OPENED

                previewOpen?.onCameraOpen()

                if(surfaceTexture != null){
                    startPreview(surfaceTexture)
                }
            }

            override fun onError() {
                // TODO "添加报错 log"
                previewOpen?.onOpenError()
            }
        })
    }

    fun startPreview(surfaceTexture: SurfaceTexture) {

        if(state != CameraState.OPENED){
            // TODO 打log,Camera还没有正常打开，不应该进行 Preview 操作
            return
        }

        val displayOrientation = windowManager.defaultDisplay.rotation * 90
        setDisplayOrientation(displayOrientation)

        cameraImpl.startPreview(surfaceTexture, object : CameraImpl.PreviewStartCallback {

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

    fun stopPreview(faceChangedCallback: FacingChangedCallback? = null) {

        cameraImpl.stopPreview(object : CameraImpl.PreviewStopCallback {
            override fun onStop() {
                state = CameraState.STOPPED
                closeCamera(faceChangedCallback)
            }

            override fun onError() {

            }

        })
    }

    fun closeCamera(faceChangedCallback: FacingChangedCallback? = null) {
        cameraImpl.closeCamera(object : CameraImpl.CameraCloseCallback {
            override fun onClose() {
                state = CameraState.RELEASED
                faceChangedCallback?.onFacingChanged()
            }

            override fun onError() {

            }
        })
    }


    /************************************** 切换相机参数函数 **********************************************/

    fun setFlash(flash: Int) {
        if (mFlash == flash) {
            return
        }
        cameraImpl.setFlash(flash)
    }

    fun setFacing(facing: Int) {
        if (mFacing == facing) {
            return
        }
        mFacing = facing

        cameraImpl.setFacing(facing)

        if (isCameraOpened()) { //前后置镜头切换的时候要关闭预览再打开
            stopPreview(object: FacingChangedCallback {
                override fun onFacingChanged() {
                    openCamera(facing)
                }
            })
        }
    }

    fun setAutoFocus(autoFocus: Boolean) {
        if (mAutoFocus == autoFocus) {
            return
        }
        cameraImpl.setAutoFocus(autoFocus)
    }

    fun setAspectRatio(ratio: AspectRatio): Boolean {
        return cameraImpl.setAspectRatio(ratio)
    }

    fun getSupportedAspectRatios(): Set<AspectRatio>{
        return cameraImpl.getSupportedAspectRatios()
    }

    fun setDisplayOrientation(displayOrientation: Int){
        cameraImpl.setDisplayOrientation(displayOrientation)
    }




    /****************************************** native函数 **********************************************/

    private external fun nativeCameraInit(): Long

}