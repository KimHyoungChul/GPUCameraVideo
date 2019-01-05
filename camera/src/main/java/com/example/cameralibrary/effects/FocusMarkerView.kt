package com.example.cameralibrary.effects

import android.animation.Animator
import android.animation.AnimatorListenerAdapter
import android.content.Context
import android.util.AttributeSet
import android.view.LayoutInflater
import android.widget.FrameLayout
import android.widget.ImageView
import com.example.cameralibrary.R

/**
 * Created by liuxuan on 2019/1/5
 */

class FocusMarkerView : FrameLayout {

    private val mFocusMarkerContainer: FrameLayout
    private val mFill: ImageView

    constructor(context: Context) : this(context, null)

    constructor(context: Context, attributeSet: AttributeSet?) : this(context, attributeSet, 0)

    constructor(context: Context, attributeSet: AttributeSet?, defStyleAttr :Int ):super(context, attributeSet, defStyleAttr)

    init {
        val view = LayoutInflater.from(context).inflate(R.layout.layout_focus_marker, this)
        mFocusMarkerContainer = view.findViewById(R.id.focusMarkerContainer)
        mFill = view.findViewById(R.id.fill)
        mFocusMarkerContainer.alpha = 0f
    }

    fun focus(mx: Float, my: Float) {
        val x = mx - mFocusMarkerContainer.width / 2
        val y = my - mFocusMarkerContainer.width / 2

        mFocusMarkerContainer.translationX = x
        mFocusMarkerContainer.translationY = y

        mFocusMarkerContainer.animate().setListener(null).cancel()
        mFill.animate().setListener(null).cancel()

        mFill.scaleX = 0f
        mFill.scaleY = 0f
        mFill.alpha = 1f

        mFocusMarkerContainer.scaleX = 1.36f
        mFocusMarkerContainer.scaleY = 1.36f
        mFocusMarkerContainer.alpha = 1f

        mFocusMarkerContainer.animate()
                .scaleX(1f).scaleY(1f)
                .setStartDelay(0).setDuration(330)
                .setListener(object : AnimatorListenerAdapter() {
                    override fun onAnimationEnd(animation: Animator) {
                        super.onAnimationEnd(animation)
                        mFocusMarkerContainer.animate().alpha(0f).setStartDelay(750).setDuration(800).setListener(null).start()
                    }
                }).start()

        mFill.animate()
                .scaleX(1f).scaleY(1f).setDuration(330)
                .setListener(object : AnimatorListenerAdapter() {
                    override fun onAnimationEnd(animation: Animator) {
                        super.onAnimationEnd(animation)
                        mFill.animate().alpha(0f).setDuration(800).setListener(null).start()
                    }
                }).start()

    }


}
