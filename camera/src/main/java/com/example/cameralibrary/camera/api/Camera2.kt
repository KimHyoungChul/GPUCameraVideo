package com.example.cameralibrary.camera.api

import android.graphics.SurfaceTexture
import com.example.cameralibrary.camera.AspectRatio
import com.example.cameralibrary.preview.PreviewImpl

/**
 * Created by liuxuan on 2018/12/27
 */
class Camera2 : CameraImpl(){
    override fun takePicture(cameraTakePicture: PreviewImpl.CameraTakePictureListener) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun openCamera(mFacing: Int, callback: CameraOpenCallback) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun closeCamera(callback: CameraCloseCallback) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun startPreview(surfaceTexture: SurfaceTexture, callback: PreviewStartCallback) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun stopPreview(callback: PreviewStopCallback) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun setPreviewImpl(mPreview: PreviewImpl) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun setFacing(facing: Int) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun setFlash(flash: Int) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun setAutoFocus(autoFocus: Boolean) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun setAspectRatio(ratio: AspectRatio): Boolean {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun getSupportedAspectRatios(): Set<AspectRatio> {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun setDisplayOrientation(displayOrientation: Int) {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }


}