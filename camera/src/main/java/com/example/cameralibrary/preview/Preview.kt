package com.example.cameralibrary.preview

import android.content.Context
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import android.widget.FrameLayout
import com.example.cameralibrary.effects.FocusMarkerView
import com.example.cameralibrary.preview.surfaceview.SurfaceViewPreview

/**
 * Created by liuxuan on 2019/1/2
 */

class Preview : FrameLayout {

    private val mPreviewImpl: PreviewImpl
    private val mFocusMarkerView: FocusMarkerView

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

    private fun creatPreview(context: Context, attributeSet: AttributeSet?, defStyleAttr: Int): PreviewImpl {
        return SurfaceViewPreview(context, this, attributeSet, defStyleAttr)
    }

    fun getView(): View? {
        return mPreviewImpl.getView()
    }

}