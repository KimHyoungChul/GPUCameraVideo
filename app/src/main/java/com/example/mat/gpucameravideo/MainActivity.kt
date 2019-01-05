package com.example.mat.gpucameravideo

import android.Manifest
import android.content.pm.PackageManager
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.support.v4.app.ActivityCompat
import android.support.v4.content.PermissionChecker
import android.view.View
import com.example.cameralibrary.preview.Preview

class MainActivity : AppCompatActivity() {

    private var preview: Preview? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        preview = findViewById(R.id.cameraview)

        if (PermissionChecker.checkSelfPermission(this@MainActivity, Manifest.permission.CAMERA) == PackageManager.PERMISSION_DENIED)  {
            ActivityCompat.requestPermissions(this@MainActivity, arrayOf(Manifest.permission.CAMERA), 100)
        }
        if (PermissionChecker.checkSelfPermission(this@MainActivity, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED)  {
            ActivityCompat.requestPermissions(this@MainActivity, arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE), 100)
        }
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
        }
    }
}
