package com.example.cameralibrary.preview

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.example.cameralibrary.camera.Camera

/**
 * Created by liuxuan on 2018/12/26
 */

class CameraSurfaceView : SurfaceView, SurfaceHolder.Callback{

    private val camera: Camera = Camera()
    private var cameraSurfaceTexture: CameraSurfaceTexture? = null

    private var nativeTextureId: Int = 0
    private var nativeOutputSurfaceAddress: Long = 0

    constructor(context : Context) : super(context)

    constructor(context: Context, attributes: AttributeSet) : super(context, attributes)

    init {
        holder.addCallback(this)
    }

    private fun genTexture(textureCallback : (nativeCamera: Long, surfaceTexture: Int) -> Unit){
        val nativeInputCameraAddress = camera.creatNativeCamera()
        nativeTextureId = nativeGenTexture(nativeInputCameraAddress)
        textureCallback(nativeInputCameraAddress, nativeTextureId)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

        if(holder != null) {
            nativeOutputSurfaceAddress = nativeSurfaceWindowInit(holder.surface) //初始化native层egl的surface表面
        }

    }

    /**
     * cameraSurfaceTexture.updateTexImage()这个方法，这个方法会在native调用egl函数去进行OES纹理的一系列渲染操作，因此
     * 要保证该方法所在的线程中，对应的egl实例（EGLContext、EGLDisplay、EGLSurface）没有问题。
     *
     * 如果存在上述对象"被系统回收（常见于onPause）"或者"OES纹理对应的线程没有egl对象（创建错线程了）"等问题，就会出现
     * checkAndUpdateEglState: invalid current EGLDisplay 的问题
     *
     * 因此我们在 updateTexImage 函数前面加上了 onSurfaceTextureAvailable 函数，就是保证nativeCamera中的
     * eglInstance已经被正确的makeCurrent了
     *
     * 在本项目中，updateTexImage()方法存在于 SurfaceHolder.Callback 线程中，因此在native中我们用了egl的共享上
     * 下文（ShareContext）,使得主Context中（对应mainlooper）的egl对象可以和 Callback线程共享OES纹理，从而实现
     * 预览帧渲染。当然你要是把 updateTexImage 也搞到mainlooper对应的线程中去做，就没有这么多事情了，本项目中主要是
     * 为了实现异步编码写入本地而做的这么一套处理
     */
    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        genTexture { nativeCamera, surfaceTexture ->
            nativeOnSurfaceChanged(nativeCamera, width, height)
            cameraSurfaceTexture = CameraSurfaceTexture(nativeCamera, surfaceTexture)
            cameraSurfaceTexture?.setOnFrameAvailableListener{
                    onSurfaceTextureAvailable(nativeCamera)
                    it.updateTexImage()
                    surfaceTextureAvailable(nativeCamera)
            }
        }

        camera.openCamera(cameraSurfaceTexture, width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        camera.stopPreview()
    }

    fun closeCamera(){
        camera.closeCamera()
    }

    private external fun nativeSurfaceWindowInit(surface: Any): Long

    private external fun nativeGenTexture(nativeCameraAddress: Long) : Int

    private external fun onSurfaceTextureAvailable(nativeCameraAddress: Long)

    private external fun surfaceTextureAvailable(nativeCameraAddress: Long)

    private external fun nativeOnSurfaceChanged(nativeCameraAddress: Long, width: Int, height: Int)
}
