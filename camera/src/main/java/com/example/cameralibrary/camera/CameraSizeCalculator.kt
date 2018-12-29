package com.example.cameralibrary.camera

/**
 * Created by liuxuan on 2018/12/28
 */

class CameraSizeCalculator(private val sizes: Array<CameraSize>) {

    fun findClosestSizeContainingTarget(target: CameraSize): CameraSize {
        sizes.sort()

        var bestSize = sizes.last()
        var bestArea = Int.MAX_VALUE
        sizes.forEach {
            if (it.width >= target.width
                    && it.height >= target.height
                    && it.getSize() < bestArea) {
                bestSize = it
                bestArea = it.getSize()
            }
        }

        return bestSize
    }

}