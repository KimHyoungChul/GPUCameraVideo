package com.example.mat.gpucameravideo

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.widget.Button
import com.example.codeclibrary.MoviePlayer
import com.example.codeclibrary.playerview.SurfacePlayerview
import android.text.TextUtils
import android.media.MediaMetadataRetriever
import android.util.Log
import android.view.SurfaceHolder


/**
 * Created by liuxuan on 2019/2/7
 */

class PlayerActivity : AppCompatActivity(), SurfacePlayerview.PreviewLifeListener {

    private var playerView: SurfacePlayerview? = null
    private var moviePlayer: MoviePlayer? = null
    private var btn: Button? = null

    private var moivePath : String = "/storage/emulated/0/test1550826781725.mp4"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_player)
        playerView = findViewById(R.id.surfacePlayerview)
        playerView?.addPreviewLifeListener(this) //设置PreView生命周期监听

        btn = findViewById(R.id.playerbtn)
        btn?.setOnClickListener {
            val metadataRetriever = MediaMetadataRetriever()
            metadataRetriever.setDataSource(moivePath)
            val widthString = metadataRetriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_WIDTH)
            if (!TextUtils.isEmpty(widthString)) {
                val width = Integer.valueOf(widthString)
                Log.e("width", width.toString())
            }
            val heightString = metadataRetriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_HEIGHT)
            if (!TextUtils.isEmpty(heightString)) {
                val height = Integer.valueOf(heightString)
                Log.e("height", height.toString())
            }
            val durationString = metadataRetriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION)
            if (!TextUtils.isEmpty(durationString)) {
                val duration = java.lang.Long.valueOf(durationString).toInt()
                Log.e("duration", duration.toString())
            }
            val bitrateString = metadataRetriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_BITRATE)
            if (!TextUtils.isEmpty(bitrateString)) {
                val bitrate = Integer.valueOf(bitrateString)
                Log.e("bitrate", bitrate.toString())
            }
            val degreeStr = metadataRetriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_ROTATION)
            if (!TextUtils.isEmpty(degreeStr)) {
                val degree = Integer.valueOf(degreeStr)
                Log.e("degree", degree.toString())
            }
            metadataRetriever.release()

            moviePlayer?.startPlayer()
        }
    }


    override fun onPreviewReady(holder: SurfaceHolder) {
        moviePlayer = MoviePlayer(moivePath)
        moviePlayer?.addTarget(playerView!!)
    }

    override fun onPreviewDestroyed(holder: SurfaceHolder) {
        moviePlayer?.surfaceDestroyed()
    }


    companion object {

        init {
            System.loadLibrary("codec-lib")
        }
    }

}