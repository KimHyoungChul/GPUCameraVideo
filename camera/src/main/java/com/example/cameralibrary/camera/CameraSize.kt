package com.example.cameralibrary.camera

/**
 * Created by liuxuan on 2018/12/27
 */
class CameraSize(val width: Int, val height: Int): Comparable<CameraSize> {
    companion object {
        operator fun invoke(size: android.hardware.Camera.Size): CameraSize{
            return CameraSize(size.width, size.height)
        }
    }

    fun getArea(): Int{
        return width * height
    }

    override fun compareTo(other: CameraSize): Int {
        return when {
            getArea() > other.getArea() -> 1
            getArea() < other.getArea() -> -1
            else -> 0
        }
    }

}