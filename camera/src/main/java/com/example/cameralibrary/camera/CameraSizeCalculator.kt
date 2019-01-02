package com.example.cameralibrary.camera

/**
 * Created by liuxuan on 2018/12/28
 */

class CameraSizeCalculator(private val sizes: Array<CameraSize>) {

    fun findBestPreviewSize(target: CameraSize): CameraSize {
        sizes.sort() //支持的预览尺寸从小到大排列

        var bestSize = sizes.last() //在宽高比一定的情况下，拍摄帧往往选择尺寸最大的，那样拍摄的图片更清楚
        var bestArea = Int.MAX_VALUE
        sizes.forEach { //从选定的长宽比支持的尺寸中，找到长和宽都大于或等于预览控件尺寸的，若是小于预览控件的宽高则会导致图像被拉伸了。
            if (it.width >= target.width && it.height >= target.height && it.getArea() < bestArea) {
                bestSize = it
                bestArea = it.getArea()
            }
        }

        return bestSize
    }

}