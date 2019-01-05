package com.example.cameralibrary.camera.api

import android.graphics.Rect
import android.graphics.SurfaceTexture
import android.hardware.Camera
import android.os.Handler
import android.support.v4.util.SparseArrayCompat
import android.util.Log
import android.view.MotionEvent
import android.view.View
import com.example.cameralibrary.camera.AspectRatio
import com.example.cameralibrary.camera.CameraParam
import com.example.cameralibrary.camera.CameraSize
import com.example.cameralibrary.camera.CameraSizeMap
import com.example.cameralibrary.preview.PreviewImpl
import java.nio.ByteBuffer
import java.util.*

/**
 * Created by liuxuan on 2018/12/27
 */
class Camera1 : CameraImpl() {

    private val TAG = this::class.java.simpleName

    private var mCamera: Camera? = null
    private var mCameraParameters: Camera.Parameters? = null
    private val mCameraInfo = Camera.CameraInfo()

    private var mFacing: Int = 0
    private var mFlash: Int = 0
    private var mAutoFocus: Boolean = false
    private var mAspectRatio: AspectRatio? = null //当前相机preview的宽高比
    private var mDisplayOrientation: Int = 0

    private val mAutofocusCallback: Camera.AutoFocusCallback? = null

    private var mShowingPreview: Boolean = false
    private var camera1state: Camera1State = Camera1State.RELEASED

    private val mPreviewSizes = CameraSizeMap()
    private val mPictureSizes = CameraSizeMap()

    private val FLASH_MODES = SparseArrayCompat<String>()

    private val FOCUS_AREA_SIZE_DEFAULT = 300
    private val FOCUS_METERING_AREA_WEIGHT_DEFAULT = 1000
    private val MIN_TIME_FOR_AUTOFOCUS: Long = 2000 //拍照时最短的自动对焦时间限制
    private val DELAY_MILLIS_BEFORE_RESETTING_FOCUS: Long = 3000 //重置焦距最短的自动对焦时间限制

    override fun setPreviewImpl(preview: PreviewImpl) {
        mPreview = preview
    }

    init {
        FLASH_MODES.put(CameraParam.FLASH_OFF, Camera.Parameters.FLASH_MODE_OFF)
        FLASH_MODES.put(CameraParam.FLASH_ON, Camera.Parameters.FLASH_MODE_ON)
        FLASH_MODES.put(CameraParam.FLASH_TORCH, Camera.Parameters.FLASH_MODE_TORCH)
        FLASH_MODES.put(CameraParam.FLASH_AUTO, Camera.Parameters.FLASH_MODE_AUTO)
        FLASH_MODES.put(CameraParam.FLASH_RED_EYE, Camera.Parameters.FLASH_MODE_RED_EYE)
    }

    private enum class Camera1State {
        OPENED,
        STARTED,
        STOPPED,
        RELEASED;
    }

    private fun isCameraOpened(): Boolean {
        return camera1state == Camera1State.OPENED || camera1state == Camera1State.STARTED
    }


    /************************************** 相机生命周期函数 **********************************************/

    override fun openCamera(mFacing: Int, callback: CameraOpenCallback) {
        handler {
            //handler函数只有一个runnable参数，因此直接用lamada表达式写在括号外面
            val currentCamera = mCamera
            if (currentCamera != null) {
                callback.onError()
                return@handler
            }

            try {
                val cameraInfo = Camera.CameraInfo()
                val cameraId = (0 until Camera.getNumberOfCameras()).find {
                    Camera.getCameraInfo(it, cameraInfo)
                    cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_BACK //相机朝向的方向
                }

                if (cameraId == null) {
                    callback.onError()
                    return@handler
                }

                val camera = Camera.open(cameraId)
                camera.setErrorCallback { error, _ ->
                    when (error) {
                        Camera.CAMERA_ERROR_EVICTED -> RuntimeException("CAMERA_ERROR_EVICTED")
                        Camera.CAMERA_ERROR_SERVER_DIED -> RuntimeException("CAMERA_ERROR_SERVER_DIED")
                        Camera.CAMERA_ERROR_UNKNOWN -> RuntimeException("CAMERA_ERROR_UNKNOWN")
                        else -> RuntimeException("Undefined camera error received, but assuming fatal.")
                    }
                }

                this.mCamera = camera

                mCameraParameters = camera.parameters

                mPreviewSizes.clear() //将相机支持的PreciewSize保存到mPreviewSizes中
                for (size in camera.parameters.supportedPreviewSizes) {
                    mPreviewSizes.add(CameraSize(size.width, size.height))
                }
                Log.i(TAG, "openCamera, supportedPreviewSizes: " + mPreviewSizes)

                mPictureSizes.clear() //将相机支持的PictureSize保存到mPreviewSizes中
                for (size in camera.parameters.supportedPictureSizes) {
                    mPictureSizes.add(CameraSize(size.width, size.height))
                }
                Log.i(TAG, "openCamera, supportedPictureSizes: " + mPictureSizes)

                //删除在图片预览大小中那些没有必要的大小，因为这些大小在输出图片中不可能有这个宽高比，预览图片、输出图片和AspectRatio三个的比例值必须一样
                val ratiosToDelete = mPreviewSizes.ratios().filterNot { mPictureSizes.ratios().contains(it) }
                for (ratio in ratiosToDelete) {
                    mPreviewSizes.remove(ratio)
                }

                adjustCameraParameters()

                camera1state = Camera1State.OPENED

                callback.onOpen(camera)
            } catch (e: Exception) {
                callback.onError()
            }
        }
    }

    override fun startPreview(surfaceTexture: SurfaceTexture, callback: PreviewStartCallback) {
        handler {
            val currentCamera = mCamera
            if (currentCamera == null) {
                callback.onError()
                return@handler
            }

            try {
                currentCamera.setPreviewTexture(surfaceTexture)
//                //使用此方法注册预览回调接口时，会将下一帧数据回调给onPreviewFrame()方法，之后这个接口将被销毁。也就是只会回调一次预览帧数据。
                currentCamera.setOneShotPreviewCallback { _, _ ->
                    callback.onStart()
                }
                //一旦使用此方法注册预览回调接口，onPreviewFrame()方法会一直被调用，直到camera preview销毁
                currentCamera.setPreviewCallback { bytes: ByteArray, _ ->
                    callback.onPreviewFrame(ByteBuffer.wrap(bytes))
                }
                currentCamera.startPreview()

                mShowingPreview = true
                camera1state = Camera1State.STARTED

            } catch (e: Exception) {
                callback.onError()
            }
        }
    }

    override fun stopPreview(callback: PreviewStopCallback) {
        handler {
            val currentCamera = mCamera
            if (currentCamera == null) {
                callback.onError()
                return@handler
            }

            try {
                currentCamera.setPreviewCallback(null)
                currentCamera.stopPreview()

                mShowingPreview = false
                camera1state = Camera1State.STOPPED
            } catch (e: Exception) {
                callback.onError()
            } finally {
                callback.onStop()
            }
        }
    }

    override fun closeCamera(callback: CameraCloseCallback) {

        handler {
            val currentCamera = mCamera
            if (currentCamera == null) {
                callback.onError()
                return@handler
            }

            try {
                currentCamera.release()
            } catch (e: Exception) {
                callback.onError()
            } finally {
                mCamera = null
                callback.onClose()
            }
        }
    }


    /****************************************** 拍照函数 *************************************************/

    override fun takePicture() {

    }


    /************************************** 切换相机参数函数 **********************************************/

    /**
     * 切换闪光灯模式
     */
    override fun setFlash(flash: Int) {
        if (flash == mFlash) {
            return
        }
        if (setFlashInternal(flash)) {
            mCamera?.parameters = mCameraParameters
        }
    }

    /**
     * 切换前后置镜头，考虑到有些手机上切换前后置镜头可能存在的闪退问题，因此这里将关闭再打开的过程放到了异步线程中，
     * 也就是在Camera类中调用stopPreview和openCamera，这里只是设置一下 mFacing 的值即可
     */
    override fun setFacing(facing: Int) {
        if (mFacing == facing) {
            return
        }
        mFacing = facing
    }

    override fun setAutoFocus(autoFocus: Boolean) {
        if (mAutoFocus == autoFocus) {
            return
        }
        if (setAutoFocusInternal(autoFocus)) {
            mCamera?.parameters = mCameraParameters
        }
    }

    /**
     * 设置预览界面的宽高比例
     */
    override fun setAspectRatio(ratio: AspectRatio): Boolean {
        if (mAspectRatio == null || !isCameraOpened()) {
            mAspectRatio = ratio //第一次在SurfaceViewPreview中初始化的时候走的这里，此时设置的是16：9比例的默认大小
            return true
        } else if (mAspectRatio != ratio) { //这个对应我们切换preview界面比例时候的操作，此时相机已经打开
            val sizes = mPreviewSizes.sizes(ratio) // 查询相机支不支持设置的比例
            return if (sizes == null) { //说明previewSize并不支持这个比例，这个时候应该属于bug，不支持的比例就不应该展示出来
                // TODO 打log，并提醒用户相机并不支持这个比例
                false
            } else {
                mAspectRatio = ratio
                adjustCameraParameters()
                true
            }
        }
        return false
    }

    override fun getSupportedAspectRatios(): Set<AspectRatio> { //求出mPreviewSizes中那些在mPictureSizes中也存在的比例列表
        val idealAspectRatios = mPreviewSizes
        val ratiosToDelete = idealAspectRatios.ratios().filter { mPictureSizes.sizes(it) == null }
        for (ratio in ratiosToDelete) {
            idealAspectRatios.remove(ratio)
        }
        return idealAspectRatios.ratios()
    }

    /**
     * 设置屏幕方向，用于在旋转屏幕的时候调整预览界面的方向，当然本项目中我们的预览界面直接是在native层用的
     * OES纹理渲染的，所以需要根据这个 displayOrientation 的值 改变texture矩阵,比如右旋左旋等
     */
    override fun setDisplayOrientation(displayOrientation: Int) {
        if (mDisplayOrientation == displayOrientation) {
//            Log.i(TAG, "Camera1 setDisplayOrientation, displayOrientation = %d, not changed", displayOrientation)
            return
        }
        mDisplayOrientation = displayOrientation
        if (isCameraOpened()) {
            val rotation = calcCameraRotation(displayOrientation)
            mCameraParameters?.setRotation(rotation)
            mCamera?.parameters = mCameraParameters
            if (mShowingPreview) {
                mCamera?.stopPreview()
            }
            val orientation = calcDisplayOrientation(displayOrientation)
            mCamera?.setDisplayOrientation(orientation)
            if (mShowingPreview) {
                mCamera?.startPreview()
            }
//            CameraLog.i(TAG, "Camera1 setDisplayOrientation, new orientation = %d, camera rotation = %d, camera orientation = %d", displayOrientation, rotation, orientation)
        }
    }


    /**
     * 设置自动对焦
     */
    private fun setAutoFocusInternal(autoFocus: Boolean): Boolean {
        mAutoFocus = autoFocus
        if (isCameraOpened() && mCameraParameters != null) {
            val modes = mCameraParameters!!.supportedFocusModes
            if (autoFocus && modes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
                attachFocusTapListener()
                mCameraParameters!!.focusMode = Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE
//                Log.i(TAG, "setAutoFocusInternal, FOCUS_MODE_CONTINUOUS_PICTURE, autoFocus = true")
            } else if (modes.contains(Camera.Parameters.FOCUS_MODE_FIXED)) {
                detachFocusTapListener()
                mCameraParameters!!.focusMode = Camera.Parameters.FOCUS_MODE_FIXED
//                Log.i(TAG, "setAutoFocusInternal, FOCUS_MODE_FIXED, autoFocus = %s", autoFocus)
            } else if (modes.contains(Camera.Parameters.FOCUS_MODE_INFINITY)) {
                detachFocusTapListener()
                mCameraParameters!!.focusMode = Camera.Parameters.FOCUS_MODE_INFINITY
//                Log.i(TAG, "setAutoFocusInternal, FOCUS_MODE_INFINITY, autoFocus = %s", autoFocus)
            } else {
                detachFocusTapListener()
                mCameraParameters!!.focusMode = modes[0] //getSupportedFocusModes方法返回的列表至少有一个元素
//                Log.i(TAG, "setAutoFocusInternal, mode = %s, autoFocus = %s", modes[0], autoFocus)
            }
            return true
        } else {
//            Log.i(TAG, "setAutoFocusInternal, camera not open, autoFocus = %s", autoFocus)
            return false
        }
    }

    /**
     * 设置闪光灯
     */
    private fun setFlashInternal(flash: Int): Boolean {
        if (isCameraOpened() && mCameraParameters != null) {
            val modes = mCameraParameters!!.supportedFlashModes//如果不支持设置闪光灯的话，getSupportedFlashModes方法会返回null
            val mode = FLASH_MODES.get(flash)
            if (modes != null && modes.contains(mode)) {
                mCameraParameters!!.flashMode = mode
                mFlash = flash
//                Log.i(TAG, "setFlashInternal, flash = %d", flash)
                return true
            }
            val currentMode = FLASH_MODES.get(mFlash)
            if (modes == null || !modes.contains(currentMode)) {
                mCameraParameters!!.flashMode = Camera.Parameters.FLASH_MODE_OFF
                mFlash = CameraParam.FLASH_OFF
                Log.i(TAG, "setFlashInternal, flash is FLASH_OFF")
                return true
            }
            return false
        } else {
            mFlash = flash
//            Log.i(TAG, "setFlashInternal, camera not open, flash = %d", flash)
            return false
        }
    }


    /**
     * 手动对焦,参考方案：
     * https://github.com/lin18/cameraview/commit/47b8a4e493cdb5f1085333577d55b749443047e9
     */
    private fun attachFocusTapListener() {
        if (mPreview == null) {
            // TODO 打log报错
            return
        }
        mPreview?.getView()?.setOnTouchListener(View.OnTouchListener { _, event ->
            if (event.action == MotionEvent.ACTION_UP) {
                if (mCamera != null) {
                    val parameters = mCamera!!.parameters
                    val focusMode = parameters.focusMode
                    val rect = calculateFocusArea(event.x, event.y)
                    val meteringAreas = ArrayList<Camera.Area>()
                    meteringAreas.add(Camera.Area(rect, FOCUS_METERING_AREA_WEIGHT_DEFAULT))

                    if (parameters.maxNumFocusAreas != 0 && focusMode != null &&
                            (focusMode == Camera.Parameters.FOCUS_MODE_AUTO ||
                                    focusMode == Camera.Parameters.FOCUS_MODE_MACRO ||
                                    focusMode == Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE ||
                                    focusMode == Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {

                        if (!parameters.supportedFocusModes.contains(Camera.Parameters.FOCUS_MODE_AUTO)) {
                            return@OnTouchListener false //cannot autoFocus
                        }
                        parameters.focusMode = Camera.Parameters.FOCUS_MODE_AUTO
                        parameters.focusAreas = meteringAreas
                        if (parameters.maxNumMeteringAreas > 0) {
                            parameters.meteringAreas = meteringAreas
                        }
                        mCamera!!.parameters = parameters

                        try {
                            mCamera!!.autoFocus({ success, camera ->
                                resetFocus(success, camera)
                            })
                        } catch (error: Exception) {
                            //ignore this exception
                            Log.e(TAG, "attachFocusTapListener, autofocus fail case 1", error)
                        }

                    } else if (parameters.maxNumMeteringAreas > 0) {
                        if (!parameters.supportedFocusModes.contains(Camera.Parameters.FOCUS_MODE_AUTO)) {
                            return@OnTouchListener false //cannot autoFocus
                        }
                        parameters.focusMode = Camera.Parameters.FOCUS_MODE_AUTO
                        parameters.focusAreas = meteringAreas
                        parameters.meteringAreas = meteringAreas
                        mCamera!!.parameters = parameters

                        try {
                            mCamera!!.autoFocus({ success, camera ->
                                resetFocus(success, camera)
                            })
                        } catch (error: Exception) {
                            //ignore this exception
                            Log.e(TAG, "attachFocusTapListener, autofocus fail case 2", error)
                        }

                    } else {
                        try {
                            mCamera!!.autoFocus({ success, camera ->
                                mAutofocusCallback?.onAutoFocus(success, camera)
                            })
                        } catch (error: Exception) {
                            //ignore this exception
                            Log.e(TAG, "attachFocusTapListener, autofocus fail case 3", error)
                        }

                    }
                }
            }
            true
        })
    }

    private fun detachFocusTapListener() {
        if (mPreview != null && mPreview!!.getView() != null) {
            mPreview!!.getView()?.setOnTouchListener(null)
        }
    }

    private val mHandler = Handler()

    private fun resetFocus(success: Boolean, camera: Camera?) {
        mHandler.removeCallbacksAndMessages(null)
        mHandler.postDelayed({
            if (camera != null) {
                camera.cancelAutoFocus()
                try {
                    val params = camera.parameters
                    if (params != null && !params.focusMode.equals(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE, ignoreCase = true)) {
                        if (params.supportedFocusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
                            params.focusMode = Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE
                            params.focusAreas = null
                            params.meteringAreas = null
                            camera.parameters = params
                        }
                    }
                } catch (error: Exception) {
                    //ignore this exception
                    Log.e(TAG, "resetFocus, camera getParameters or setParameters fail", error)
                }

                mAutofocusCallback?.onAutoFocus(success, camera)
            }
        }, DELAY_MILLIS_BEFORE_RESETTING_FOCUS)
    }


    /************************************** 相机参数相关计算函数 *******************************************/

    private fun adjustCameraParameters() {
        var sizes = mPreviewSizes.sizes(mAspectRatio) //这里size为空也可能表示mAspectRatio为空，也就是宽高比这个值还没有初始化
        if (sizes == null) { // 不支持这个宽高比或者mAspectRatio还没有初始化
//            Log.i(TAG, "adjustCameraParameters, ratio[%s] is not supported", mAspectRatio)
            mAspectRatio = chooseAspectRatio() //返回16：9或者4：3或者mPreviewSizes支持的第一个宽高比例
            sizes = mPreviewSizes.sizes(mAspectRatio) //此时sizes肯定不为空了，除非mPreviewSizes为空
//            Log.i(TAG, "adjustCameraParameters, change to ratio to %s", mAspectRatio)
        }
        val previewSize = choosePreviewSize(sizes) //选择预览帧的大小
        if (previewSize == null) {
            Log.e("adjustCameraParameters", "previewSize is null")
            return
        }

        val pictureSize = choosePictureSize() //选择拍照图片的分辨率
        if (pictureSize == null) {
            Log.e("adjustCameraParameters", "pictureSize is null")
            return
        }

        if (mShowingPreview) {
            mCamera?.stopPreview()//在重新设置CameraParameters之前需要停止预览
        }

        mCameraParameters?.setPreviewSize(previewSize.getWidth(), previewSize.getHeight())
        mCameraParameters?.setPictureSize(pictureSize.getWidth(), pictureSize.getHeight())
        mCameraParameters?.setRotation(calcCameraRotation(mDisplayOrientation))
        setAutoFocusInternal(mAutoFocus)
        setFlashInternal(mFlash)
        mCamera?.parameters = mCameraParameters

//        Log.i(TAG, "adjustCameraParameters, PreviewSize = %s, PictureSize = %s, AspectRatio = %s, AutoFocus = %s, Flash = %s", previewSize, pictureSize, mAspectRatio, mAutoFocus, mFlash)

        if (mShowingPreview) {
            mCamera?.startPreview()
        }
    }

    private fun choosePreviewSize(sizes: SortedSet<CameraSize>?): CameraSize? {
        if (mPreview == null) {
            // TODO 打log，报错，此时mPreview不应为null
            return null
        }

        if(sizes == null) {
            // TODO 打log，报错
            return null
        }

        if (!mPreview!!.previewIsReady()) { // onSurfaceChanged执行完了之后就准备好了
//            Log.i(TAG, "choosePreviewSize, preview is not ready, return size: %s", sizes.first())
            return sizes.first() // 返回最小的值，因为我们用的SortedSet，且根据CameraSize中的campare，应该是从小到大排列
        }
        val desiredWidth: Int
        val desiredHeight: Int
        val surfaceWidth = mPreview!!.getWidth()
        val surfaceHeight = mPreview!!.getHeight()
        if (isLandscape(mDisplayOrientation)) {
            desiredWidth = surfaceHeight
            desiredHeight = surfaceWidth
        } else {
            desiredWidth = surfaceWidth
            desiredHeight = surfaceHeight
        }
        /*
         * 从选定的长宽比支持的尺寸中，找到长和宽都大于或等于预览控件尺寸的，若是小于预览控件的宽高则会导致图像被拉伸了。
         * 在宽高比一定的情况下，拍摄帧往往选择尺寸最大的，那样拍摄的图片更清楚
         */
        var result: CameraSize? = null
        for (size in sizes) {
            if (desiredWidth <= size.getWidth() && desiredHeight <= size.getHeight()) {
                return size
            }
            result = size
        }
        return result
    }

    private fun chooseAspectRatio(): AspectRatio {
        val aspectRatio: AspectRatio = when {
            mPreviewSizes.ratios().contains(CameraParam.DEFAULT_ASPECT_RATIO) -> //首先看16:9是否支持
                CameraParam.DEFAULT_ASPECT_RATIO
            mPreviewSizes.ratios().contains(CameraParam.SECOND_ASPECT_RATIO) -> //再看4:3是否支持
                CameraParam.SECOND_ASPECT_RATIO
            else -> //两个都不支持的话就取它支持的第一个作为当前的宽高比
                mPreviewSizes.ratios().iterator().next()
        }
        Log.i(TAG, "chooseAspectRatio, aspect ratio changed to " + aspectRatio.toString())
        return aspectRatio
    }

    /**
     * 这里针对具体需求调整最合适的宽高比和输出图片大小，对于不同的手机而言，大部分都支持16:9(4:3)的比例，
     * 同时大部分也都支持输出1920x1080(800x600)的图片大小
     */
    private fun choosePictureSize(): CameraSize? {
        if (mAspectRatio == null){
            Log.e("choosePictureSize", "mAspectRatio is null")
            return null
        }

        val sizes = mPictureSizes.sizes(mAspectRatio) //图片支持的宽高比，用这个比例去选合适的分辨率
        if (sizes == null){
            Log.e("choosePictureSize", "sizes is null")
            return null
        }

        return when (mAspectRatio) {
            CameraParam.DEFAULT_ASPECT_RATIO -> {
                val preferedSizes = arrayOf(CameraSize(1920, 1080), CameraSize(1280, 720))//几个比较合适的输出大小
//                for (size in preferedSizes) {
//                    if (sizes.contains(size)) {
//                        return size
//                    }
//                }
                preferedSizes.firstOrNull { sizes.contains(it) } ?: getMiddleSize(sizes) //前面几个合适的大小都没有的话，那么就使用中间那个大小
            }
            CameraParam.SECOND_ASPECT_RATIO -> {
                val preferedSizes = arrayOf(CameraSize(1440, 1080),
                        CameraSize(1280, 960), CameraSize(1024, 768), CameraSize(800, 600))//几个比较合适的输出大小
                preferedSizes.firstOrNull { sizes.contains(it) } ?: getMiddleSize(sizes)
            }
            else -> {
                getMiddleSize(sizes)
            }
        }
    }

    private fun calcDisplayOrientation(screenOrientationDegrees: Int): Int {
        return if (mCameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            (360 - (mCameraInfo.orientation + screenOrientationDegrees) % 360) % 360
        } else {
            (mCameraInfo.orientation - screenOrientationDegrees + 360) % 360
        }
    }

    private fun calcCameraRotation(screenOrientationDegrees: Int): Int {
        return if (mCameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            (mCameraInfo.orientation + screenOrientationDegrees) % 360
        } else {
            val landscapeFlip = if (isLandscape(screenOrientationDegrees)) 180 else 0
            (mCameraInfo.orientation + screenOrientationDegrees + landscapeFlip) % 360
        }
    }

    /**
     * 根据屏幕旋转的度数来判断是否是横屏
     */
    private fun isLandscape(orientationDegrees: Int): Boolean {
        return orientationDegrees == CameraParam.LANDSCAPE_90 || orientationDegrees == CameraParam.LANDSCAPE_270
    }

    //前面几个合适的大小都没有的话，那么就使用中间那个大小 (即使是中间这个大小也并不能保证它满足我们的需求，比如得到的图片还是很大，但是这种情况实在太少了)
    private fun getMiddleSize(sizes: SortedSet<CameraSize>): CameraSize {
        val length = sizes.size / 2
        var i = 0
        sizes.forEach { item ->
            if (i == length) {
                return item
            }
            i++
        }
        return sizes.last()
    }

    private fun calculateFocusArea(x: Float, y: Float): Rect {
        val view = mPreview?.getView()!!

        val buffer = FOCUS_AREA_SIZE_DEFAULT / 2
        val centerX = calculateCenter(x, view.width, buffer)
        val centerY = calculateCenter(y, view.height, buffer)
        return Rect(
                centerX - buffer,
                centerY - buffer,
                centerX + buffer,
                centerY + buffer
        )
    }

    private fun calculateCenter(coord: Float, dimen: Int, buffer: Int): Int {
        val normalized = (coord / dimen * 2000 - 1000).toInt()
        return if (Math.abs(normalized) + buffer > 1000) {
            if (normalized > 0) {
                1000 - buffer
            } else {
                -1000 + buffer
            }
        } else {
            normalized
        }
    }


}