package com.example.cameralibrary.preview

import android.graphics.SurfaceTexture
import com.example.cameralibrary.camera.CameraSize

/**
 * Created by liuxuan on 2018/12/26
 */
class CameraSurfaceTexture(nativeCameraAddress: Long, surfaceTexture: Int) : SurfaceTexture(surfaceTexture){

    var size: CameraSize = CameraSize (0, 0)
        set(size){
            field = size
            previewSizeInit = true
        }

    private var previewSizeInit = false

    init {
        nativeInitSurfaceTexture(nativeCameraAddress, surfaceTexture)
    }


    private external fun nativeInitSurfaceTexture(nativeCameraAddress: Long, surfaceTexture: Int)
}