package com.example.cameralibrary.camera

/**
 * Created by liuxuan on 2019/1/2
 */
object CameraParam {

    val DEFAULT_ASPECT_RATIO = AspectRatio.of(16, 9)//如果是16:9的话显示图片的时候可以填充整个屏幕
    val SECOND_ASPECT_RATIO = AspectRatio.of(4, 3)//如果是4:3的话显示图片的时候会上下留黑很多空间

    val FACING_BACK = 0
    val FACING_FRONT = 1

    val FLASH_OFF = 0
    val FLASH_ON = 1
    val FLASH_TORCH = 2
    val FLASH_AUTO = 3
    val FLASH_RED_EYE = 4

    val LANDSCAPE_90 = 90
    val LANDSCAPE_270 = 270
}