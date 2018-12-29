package com.example.cameralibrary.camera.api

import android.graphics.SurfaceTexture
import com.example.cameralibrary.camera.CameraSize

/**
 * Created by liuxuan on 2018/12/27
 */
class Camera2 : CameraApi(){
    override fun openCamera(callback: CameraOpenCallback) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun closeCamera(callback: CameraCloseCallback) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun startPreview(surfaceTexture: SurfaceTexture, previewSize: CameraSize, displayOrientation: Int, callback: PreViewStartCallback) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun stopPreview(callback: PreviewStopCallback) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

}