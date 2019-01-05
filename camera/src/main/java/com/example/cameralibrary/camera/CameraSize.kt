package com.example.cameralibrary.camera

/**
 * Created by liuxuan on 2018/12/27
 */
class CameraSize(val mWidth: Int, val mHeight: Int): Comparable<CameraSize> {
    companion object {
        operator fun invoke(size: android.hardware.Camera.Size): CameraSize{
            return CameraSize(size.width, size.height)
        }
    }

    override fun equals(o: Any?): Boolean {
        if (o == null) {
            return false
        }
        if (this === o) {
            return true
        }
        if (o is CameraSize) {
            return mWidth == o.mWidth && mHeight == o.mHeight
        }
        return false
    }

    override fun hashCode(): Int {
        // assuming most sizes are <2^16, doing a rotate will give us perfect hashing
        return mHeight xor (mWidth shl Integer.SIZE / 2 or mWidth.ushr(Integer.SIZE / 2))
    }

    fun getWidth(): Int{
        return mWidth
    }

    fun getHeight(): Int{
        return mHeight
    }

    fun getArea(): Int{
        return mWidth * mHeight
    }

    override fun compareTo(other: CameraSize): Int {
        return when {
            getArea() > other.getArea() -> 1
            getArea() < other.getArea() -> -1
            else -> 0
        }
    }

}