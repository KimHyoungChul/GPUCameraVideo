package com.example.cameralibrary.preview

import android.content.Context
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import android.widget.FrameLayout
import com.example.baselib.GCVInput
import com.example.baselib.GCVOutput
import com.example.cameralibrary.camera.CameraParam
import com.example.cameralibrary.effects.FocusMarkerView
import com.example.cameralibrary.preview.surfaceview.SurfaceViewPreview

/**
 * Created by liuxuan on 2019/1/2
 */

class Preview : FrameLayout {

    private val mPreviewImpl: PreviewImpl
    private val mFocusMarkerView: FocusMarkerView


    companion object {

        val FLASH_OFF = CameraParam.FLASH_OFF
        val FLASH_ON = CameraParam.FLASH_ON
        val FLASH_TORCH = CameraParam.FLASH_TORCH//手电筒状态
        val FLASH_AUTO = CameraParam.FLASH_AUTO
        val FLASH_RED_EYE = CameraParam.FLASH_RED_EYE
    }

    constructor(context: Context) : this(context, null)

    constructor(context: Context, attributeSet: AttributeSet?) : this(context, attributeSet, 0)

    constructor(context: Context, attributeSet: AttributeSet?, defStyleAttr: Int) : super(context, attributeSet, defStyleAttr) {
        mPreviewImpl = creatPreview(context, attributeSet, defStyleAttr)
        mFocusMarkerView = FocusMarkerView(getContext())
        addView(mFocusMarkerView)
        addViewListener()
    }

    private fun addViewListener() {
        mFocusMarkerView.setOnTouchListener { _, motionEvent ->
            if (motionEvent.action == MotionEvent.ACTION_UP) {
                mFocusMarkerView.focus(motionEvent.x, motionEvent.y)
            }

            if (mPreviewImpl.getView() != null) {
                mPreviewImpl.getView()!!.dispatchTouchEvent(motionEvent)
            }
            true
        }
    }

    fun setFilterGroup(filterGroup: GCVInput){
        mPreviewImpl.setFilterGroup(filterGroup)
    }

    fun addPreviewLifeListener(previewLife: PreviewLifeListener){
        mPreviewImpl.setPreviewStartListener(previewLife)
    }

    private fun creatPreview(context: Context, attributeSet: AttributeSet?, defStyleAttr: Int): PreviewImpl {
        return SurfaceViewPreview(context, this, attributeSet, defStyleAttr)
    }

    fun getView(): View? {
        return mPreviewImpl.getView()
    }

    fun getFacing(): Int {
        return mPreviewImpl.getFacing()
    }

    fun setFacing(facing: Int){
        mPreviewImpl.setFacing(facing)
    }

    fun setRecorder(output : GCVOutput){
        mPreviewImpl.setRecorder(output)
    }


    interface PreviewLifeListener {
        fun onPreviewCreated()
        fun onPreviewChanged(width: Int, height: Int)
        fun onPreviewReady()
        fun onPreviewDestory()
    }

}