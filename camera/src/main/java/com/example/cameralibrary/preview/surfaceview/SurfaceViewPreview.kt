package com.example.cameralibrary.preview.surfaceview

import android.content.Context
import android.graphics.SurfaceTexture
import android.util.AttributeSet
import android.view.*
import com.example.baselib.GCVInput
import com.example.baselib.GCVOutput
import com.example.cameralibrary.R
import com.example.cameralibrary.camera.AspectRatio
import com.example.cameralibrary.camera.CameraParam
import com.example.cameralibrary.preview.Preview
import com.example.cameralibrary.preview.PreviewImpl
import com.example.cameralibrary.preview.PreviewImpl.SurfaceListener
import com.example.cameralibrary.preview.PreviewImpl.CameraOpenListener
import com.example.cameralibrary.preview.PreviewImpl.CameraPreviewListener

/**
 * Created by liuxuan on 2019/1/2
 */
class SurfaceViewPreview(context: Context,
                         parent: ViewGroup,
                         attrs: AttributeSet?,
                         defStyleAttr: Int) : PreviewImpl(context),
        GCVOutput, SurfaceListener, CameraOpenListener, CameraPreviewListener {

    private var mSurfaceview: SurfaceView? = null
    private var mHolder: SurfaceHolder? = null

    private var mFilterGroup: GCVInput? = null

    private var mFacing: Int = 0

    private var mContext: Context? = null
    private var mAttrs: AttributeSet? = null
    private var mDefStyleAttr: Int = 0

    private var mPreviewLife: Preview.PreviewLifeListener? = null

    init {
        /*
         * 设置本类引用，本来是不想让Camera持有View引用的，现在看来是不行(否则会非常麻烦)，后面想办法重构吧
         */
        mCamera.setPreviewImpl(this)

        mAttrs = attrs
        mContext = context
        mDefStyleAttr = defStyleAttr

        val view = View.inflate(context, R.layout.surface_preview, parent)
        mSurfaceview = view.findViewById(R.id.surface_view)

        mHolder = mSurfaceview?.holder
        mHolder?.addCallback(SurfaceCallback(mCamera, this))
    }

    override fun getView(): View? {
        return mSurfaceview
    }


    /**
     * 设置滤镜组
     */
    override fun setFilterGroup(filterGroup: GCVInput) {
        mFilterGroup = filterGroup
        mFilterGroup?.addTarget(this@SurfaceViewPreview)
        mCamera.addTarget(mFilterGroup as GCVOutput)
    }

    /**
     * 添加录像组件。注意如果需要有滤镜效果，则录像组件需要挂在滤镜组上
     */
    override fun setRecorder(movieRecorder: GCVOutput) {

        mFilterGroup?.addTarget(movieRecorder)
    }

    override fun setFacing(facing: Int){
        mCamera.setFacing(facing)
    }

    override fun getFacing(): Int {
        return mCamera.getCameraFacing()
    }

    override fun previewIsReady(): Boolean { //onSurfaceChanged执行完了之后就准备好了
        return getWidth() != 0 && getHeight() != 0
    }

    private var nativeOutputSurfaceViewAddress: Long = 0L

    override fun nativeOutput(): Long {
        return if(nativeOutputSurfaceViewAddress != 0L ){
            nativeOutputSurfaceViewAddress
        }else{
            0L
        }
    }


    /******************************** SurfaceCallback生命周期 ********************************************/
    /**
     * 相机的生命周期应当由Surface来管理，不应当开放给顶层的View,但是我们可以通过
     */
    override fun onSurfaceCreated(nativeOutputAddress: Long) {
        nativeOutputSurfaceViewAddress = nativeOutputAddress

        mPreviewLife?.onPreviewCreated()
    }

    override fun onSurfaceChanged(surfaceTexture: SurfaceTexture?, width: Int, height: Int) {
        setPreviewSize(width, height)

        if(surfaceTexture != null){
            mCamera.openCamera(mFacing, surfaceTexture, this, this)
        }

        mPreviewLife?.onPreviewChanged(width, height)
    }

    override fun onSurfaceDestory() {
        mCamera.stopPreview()
        mPreviewLife?.onPreviewCreated()
    }


    /******************************** Camera 生命周期 ********************************************/

    override fun onCameraOpen() {
        if(mAttrs != null && mContext != null) {
            initCamera(mContext!!, mAttrs!!, mDefStyleAttr)
        }
    }

    override fun onOpenError() {

    }

    override fun onPreviewStart() {
        mPreviewLife?.onPreviewReady()
    }

    override fun onPreviewError() {

    }

    /*********************************************************************************************/



    override fun setPreviewStartListener(previewLife: Preview.PreviewLifeListener) {
        mPreviewLife = previewLife
    }


    private fun initCamera(context: Context, attrs: AttributeSet, defStyleAttr: Int){
        val typedArray = context.obtainStyledAttributes(attrs, R.styleable.CameraView,
                defStyleAttr, R.style.Widget_CameraView)

        mFacing = typedArray.getInt(R.styleable.CameraView_facing, CameraParam.FACING_BACK)

        val aspectRatio = typedArray.getString(R.styleable.CameraView_aspectRatio)
        if (aspectRatio != null) {
            mCamera.setAspectRatio(AspectRatio.parse(aspectRatio))
        } else {
            mCamera.setAspectRatio(CameraParam.DEFAULT_ASPECT_RATIO)//默认宽高比是16:9
        }

        mCamera.setFacing(mFacing) //默认后置摄像头
        mCamera.setAutoFocus(typedArray.getBoolean(R.styleable.CameraView_autoFocus, true))//默认自动对焦模式
        mCamera.setFlash(typedArray.getInt(R.styleable.CameraView_flash, CameraParam.FLASH_OFF))//默认关闭闪光灯

        typedArray.recycle()
    }


}