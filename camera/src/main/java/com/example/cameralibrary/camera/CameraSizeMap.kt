package com.example.cameralibrary.camera

import android.support.v4.util.ArrayMap
import java.util.*

/**
 * Created by liuxuan on 2019/1/2
 *
 * 从宽高比到图像size的映射，key是宽高比，value是满足这个宽高比的图像size集合
 */

class CameraSizeMap {

    private val mRatios = ArrayMap<AspectRatio, SortedSet<CameraSize>>()

    val isEmpty: Boolean
        get() = mRatios.isEmpty

    /**
     * Add a new [Size] to this collection.
     *
     * @param size The size to add.
     * @return `true` if it is added, `false` if it already exists and is not added.
     */
    fun add(size: CameraSize): Boolean {
        /*
        for (ratio in mRatios.keys) {
            if (ratio.matches(size)) {
                val sizes = mRatios[ratio]
                if (sizes != null) {
                    return if (sizes.contains(size)) {
                        false
                    } else {
                        sizes.add(size)
                        true
                    }
                }
            }
        }*/
        mRatios.keys
                .filter { it.matches(size) }
                .mapNotNull { mRatios[it] }
                .forEach {
                    return if (it.contains(size)) {
                        false
                    } else {
                        it.add(size)
                        true
                    }
                }
        // None of the existing ratio matches the provided size; add a new key
        val sizes = TreeSet<CameraSize>()
        sizes.add(size)
        mRatios.put(AspectRatio.of(size.getWidth(), size.getHeight()), sizes)
        return true
    }

    /**
     * Removes the specified aspect ratio and all sizes associated with it.
     */
    fun remove(ratio: AspectRatio) {
        mRatios.remove(ratio)
    }

    fun ratios(): Set<AspectRatio> {
        return mRatios.keys
    }

    fun sizes(ratio: AspectRatio?): SortedSet<CameraSize>? {
        if(ratio != null) {
            return mRatios[ratio]
        }
        return null
    }

    fun clear() {
        mRatios.clear()
    }

    override fun toString(): String {
        val builder = StringBuilder("")
        val ratioSet = ratios()
        for (ratio in ratioSet) {
            val sizeSet = sizes(ratio)
            builder.append("[").append(ratio.toString()).append("]:{")
            if (sizeSet != null) {
                for (size in sizeSet) {
                    builder.append(size.toString()).append(", ")
                }
            }
            builder.append("}; ")
        }
        return builder.toString()
    }
}
