package com.example.mat.gpucameravideo

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.widget.Button
import com.example.codeclibrary.MoviePlayer
import com.example.codeclibrary.playerview.SurfacePlayerview

/**
 * Created by liuxuan on 2019/2/7
 */

class PlayerActivity : AppCompatActivity(), SurfacePlayerview.PreviewLifeListener {

    private var playerView: SurfacePlayerview? = null
    private var moviePlayer: MoviePlayer? = null
    private var btn: Button? = null

    private var moivePath : String = "/storage/emulated/0/test1536047840381.mp4"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_player)
        playerView = findViewById(R.id.surfacePlayerview)
        playerView?.addPreviewLifeListener(this) //设置PreView生命周期监听

        btn = findViewById(R.id.playerbtn)
        btn?.setOnClickListener {
            moviePlayer?.startPlayer()
        }
    }

    override fun onPreviewReady() {
        moviePlayer = MoviePlayer(moivePath)
        moviePlayer?.addTarget(playerView!!)
    }


    companion object {

        init {
            System.loadLibrary("codec-lib")
        }
    }

}