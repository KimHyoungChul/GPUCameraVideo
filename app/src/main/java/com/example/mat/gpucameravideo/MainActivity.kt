package com.example.mat.gpucameravideo

import android.Manifest
import android.content.pm.PackageManager
import android.graphics.SurfaceTexture
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.support.v4.app.ActivityCompat
import android.support.v4.content.PermissionChecker
import android.support.v7.widget.Toolbar
import android.view.Menu
import android.view.MenuItem
import android.view.View
import com.example.cameralibrary.camera.CameraParam
import com.example.cameralibrary.preview.Preview
import com.example.cameralibrary.preview.PreviewImpl
import com.example.codeclibrary.MovieRecorder

class MainActivity : AppCompatActivity(), Preview.PreviewReadyListener{

    private var preview: Preview? = null
    private var movieRecorder: MovieRecorder? = null

    private val FLASH_OPTIONS = intArrayOf(Preview.FLASH_AUTO, Preview.FLASH_OFF, Preview.FLASH_ON)
    private val FLASH_ICONS = intArrayOf(R.drawable.ic_flash_auto, R.drawable.ic_flash_off, R.drawable.ic_flash_on)
    private val FLASH_TITLES = intArrayOf(R.string.flash_auto, R.string.flash_off, R.string.flash_on)

    private val filePath = Environment.getExternalStorageDirectory().path + "/test"
    private val savePath = filePath + System.currentTimeMillis() + ".mp4"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        preview = findViewById(R.id.cameraview)
        preview?.addPreviewReadyListener(this)

        val toolbar = findViewById<View>(R.id.toolbar) as Toolbar
        setSupportActionBar(toolbar)
        val actionBar = supportActionBar
        actionBar?.setDisplayShowTitleEnabled(false)

        if (PermissionChecker.checkSelfPermission(this@MainActivity, Manifest.permission.CAMERA) == PackageManager.PERMISSION_DENIED)  {
            ActivityCompat.requestPermissions(this@MainActivity, arrayOf(Manifest.permission.CAMERA), 100)
        }
        if (PermissionChecker.checkSelfPermission(this@MainActivity, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED)  {
            ActivityCompat.requestPermissions(this@MainActivity, arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE), 100)
        }
    }

    /**
     * 当相机的Preview准备好了之后才能开始添加录制组件，不然录得视频最开始有几帧是黑屏的！！！
     */
    override fun onPreviewReady() {
        movieRecorder = MovieRecorder(savePath, 720, 1280, (720.0 * 1280.0 * 6.51).toLong(), 0)
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem?): Boolean {
        when(item?.itemId){
            R.id.aspect_ratio -> {
                preview?.setRecorder(movieRecorder!!)
                movieRecorder?.startRecorder()
                return true
            }

            R.id.switch_flash -> {
                movieRecorder?.FinishRecorder()
                return true
            }

            R.id.switch_camera -> {
                val facing = preview?.getFacing()
                preview?.setFacing(
                        if (facing == CameraParam.FACING_FRONT)
                            CameraParam.FACING_BACK
                        else
                            CameraParam.FACING_FRONT
                )
                return true
            }

        }
        return super.onOptionsItemSelected(item)
    }

    /**
     * 在Activity的生命周期中，对SurfaceView的可见性进行设置，触发surfaceView的生命周期变化（surfaceView在可见时，
     * 会执行surfaceCreated方法，不可见时会自动销毁，执行surfaceDestroyed方法），从而更好的管理Camera的生命周期，
     * 在相机不可见时释放相机资源
     */
    override fun onResume() {
        super.onResume()
        preview?.getView()?.visibility = View.VISIBLE
    }

    override fun onPause() {
        super.onPause()
        preview?.getView()?.visibility = View.INVISIBLE
    }


    companion object {

        init {
            System.loadLibrary("codec-lib")
            System.loadLibrary("yuv")
        }
    }
}
