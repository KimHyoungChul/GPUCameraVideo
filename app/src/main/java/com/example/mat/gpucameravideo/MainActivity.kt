package com.example.mat.gpucameravideo

import android.Manifest
import android.content.pm.PackageManager
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.support.v4.app.ActivityCompat
import android.support.v4.content.PermissionChecker
import com.example.cameralibrary.preview.CameraSurfaceView

class MainActivity : AppCompatActivity() {

    private var cameraView: CameraSurfaceView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        cameraView = findViewById(R.id.cameraview)

        if (PermissionChecker.checkSelfPermission(this@MainActivity, Manifest.permission.CAMERA) == PackageManager.PERMISSION_DENIED)  {
            ActivityCompat.requestPermissions(this@MainActivity, arrayOf(Manifest.permission.CAMERA), 100)
        }
        if (PermissionChecker.checkSelfPermission(this@MainActivity, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED)  {
            ActivityCompat.requestPermissions(this@MainActivity, arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE), 100)
        }
    }


    override fun onStop() {
        super.onStop()

        cameraView?.closeCamera()
    }


    companion object {

        init {
            System.loadLibrary("codec-lib")
        }
    }
}
