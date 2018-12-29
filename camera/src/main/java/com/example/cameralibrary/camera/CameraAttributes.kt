package com.example.cameralibrary.camera

import android.hardware.Camera

/**
 * Created by liuxuan on 2018/12/27
 * Email : lx13@meitu.com
 */

class CameraAttributes(cameraInfo: Camera.CameraInfo, cameraParameters: Camera.Parameters) {

    val sensorOrientation: Int = cameraInfo.orientation

    val previewSizes: Array<CameraSize> = cameraParameters.getPreviewSizes()

    private fun Camera.Parameters.getPreviewSizes(): Array<CameraSize> {
        return supportedPreviewSizes.map { CameraSize(it) }.toTypedArray()
    }

}