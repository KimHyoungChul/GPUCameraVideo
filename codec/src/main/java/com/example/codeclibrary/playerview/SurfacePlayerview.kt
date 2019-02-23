package com.example.codeclibrary.playerview

import android.content.Context
import android.util.AttributeSet
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.example.baselib.GCVOutput

/**
 * Created by liuxuan on 2019/2/7
 */
class SurfacePlayerview: SurfaceView, SurfaceHolder.Callback, GCVOutput {

    private var lifeListener:PreviewLifeListener? = null

    private var nativeOutputSurfaceAddress: Long = 0

    constructor(context: Context) : this(context, null)

    constructor(context: Context, attributeSet: AttributeSet?) : this(context, attributeSet, 0)

    constructor(context: Context, attributeSet: AttributeSet?, defStyleAttr: Int) : super(context, attributeSet, defStyleAttr){
        holder.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        if(holder != null) {
            nativeOutputSurfaceAddress = nativeSurfaceWindowInit(holder.surface) //初始化native层egl的surface表面
        }

        if (holder != null) {
            lifeListener?.onPreviewReady(holder)
        }
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        nativeSurfaceWindowDestroyed(nativeOutputSurfaceAddress)

        if (holder != null) {
            lifeListener?.onPreviewDestroyed(holder)
        }
    }

    fun addPreviewLifeListener(previewLife: PreviewLifeListener){
        lifeListener = previewLife
    }

    fun getSurface(): Surface {
        return holder.surface
    }


    override fun nativeOutput(): Long {
        return nativeOutputSurfaceAddress
    }

    private external fun nativeSurfaceWindowInit(surface: Any): Long

    private external fun nativeSurfaceWindowDestroyed(nativeOutputSurfaceAddress: Long)

    interface PreviewLifeListener{
        fun onPreviewReady(holder: SurfaceHolder)
        fun onPreviewDestroyed(holder: SurfaceHolder)
    }

}