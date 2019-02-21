package com.example.cameralibrary.camera

import android.hardware.Camera

/**
 * Created by liuxuan on 2018/12/27
 * Email : lx13@meitu.com
 */

class CameraAttributes(cameraInfo: Camera.CameraInfo, cameraParameters: Camera.Parameters) {

//    val mPreviewSizes: CameraSizeMap  =  CameraSizeMap()

    /**
     * orientation 表示相机图像的方向。它的值是相机图像顺时针旋转到设备自然方向一致时的角度。
     *
     * 对于竖屏应用来说，后置相机传感器是横屏安装的，当你面向屏幕时：
     * 如果后置相机传感器顶边和设备自然方向的右边是平行的，那么后置相机的 orientation 是 90。
     * 如果是前置相机传感器顶边和设备自然方向的右边是平行的，则前置相机的 orientation 是 270。
     *
     * 对于前置和后置相机传感器 orientation 是不同的，在不同的设备上也可能会有不同的值。
     */
    val sensorOrientation: Int = cameraInfo.orientation

    val previewSizes: Array<CameraSize> = cameraParameters.supportedPreviewSizes.map { CameraSize(it) }.toTypedArray()

//    init {
//        for (size in cameraParameters.supportedPreviewSizes) {
//            mPreviewSizes.add(CameraSize(size.width, size.height))
//        }
//    }

}