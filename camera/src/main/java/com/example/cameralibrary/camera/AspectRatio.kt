package com.example.cameralibrary.camera

import android.support.v4.util.SparseArrayCompat

/**
 * 摄像头的宽高比
 *
 * Created by liuxuan on 2019/1/2
 */
class AspectRatio(x: Int, y: Int) : Comparable<AspectRatio>{

    private var mX: Int = x //宽
    private var mY: Int = y //高

    companion object {

        private val sCache = SparseArrayCompat<SparseArrayCompat<AspectRatio>>(16)

        fun of(x: Int, y: Int): AspectRatio {
            var x = x
            var y = y
            val gcd = gcd(x, y)
            x /= gcd
            y /= gcd
            var arrayX: SparseArrayCompat<AspectRatio>? = sCache.get(x)

            return if (arrayX == null) {
                val ratio = AspectRatio(x, y)
                arrayX = SparseArrayCompat()
                arrayX.put(y, ratio)
                sCache.put(x, arrayX)
                ratio
            } else {
                var ratio: AspectRatio? = arrayX.get(y)
                if (ratio == null) {
                    ratio = AspectRatio(x, y)
                    arrayX.put(y, ratio)
                }
                ratio
            }
        }

        fun parse(s: String): AspectRatio {
            val position = s.indexOf(':')
            if (position == -1) {
                throw IllegalArgumentException("Malformed aspect ratio: " + s)
            }
            try {
                val x = Integer.parseInt(s.substring(0, position))
                val y = Integer.parseInt(s.substring(position + 1))
                return AspectRatio.of(x, y)
            } catch (e: NumberFormatException) {
                throw IllegalArgumentException("Malformed aspect ratio: " + s, e)
            }

        }

        private fun gcd(a: Int, b: Int): Int {
            var a = a
            var b = b
            while (b != 0) {
                val c = b
                b = a % b
                a = c
            }
            return a
        }
    }

    fun getX(): Int {
        return mX
    }

    fun getY(): Int {
        return mY
    }

    fun matches(size: CameraSize): Boolean {
        val gcd = gcd(size.getWidth(), size.getHeight())
        val x = size.getWidth() / gcd
        val y = size.getHeight() / gcd
        return mX == x && mY == y
    }

    override fun compareTo(other: AspectRatio): Int {
        if (equals(other)) {
            return 0
        } else if (toFloat() - other.toFloat() > 0) {
            return 1
        }
        return -1
    }

    private fun toFloat(): Float {
        return mX.toFloat() / mY
    }

    fun inverse(): AspectRatio {
        return AspectRatio.of(mY, mX)
    }

}