package com.example.codeclibrary

import android.media.MediaCodecInfo
import android.media.MediaCodecList
import android.media.MediaFormat
import android.os.Build
import android.util.Log
import com.example.baselib.GCVOutput

/**
 * Created by liuxuan on 2019/1/10
 */

class MovieRecorder(savepath: String, width: Int, height: Int, bitRate: Long, outputOrientation: Int) : GCVOutput{

    var movieRecorderAddress: Long = 0L

    init{
        var colorFormat: Int = 0
        var bitRateMode: Int = -1

        val numCodecs = MediaCodecList.getCodecCount() // 获取所有支持编解码器数量
        var codecInfo: MediaCodecInfo? = null

        var i = 0
        while (i < numCodecs && codecInfo == null) {
            val info: MediaCodecInfo = MediaCodecList.getCodecInfoAt(i) // 编解码器相关性信息存储在MediaCodecInfo中
            if (info.isEncoder) { // 判断是否为编码器
                val types = info.supportedTypes // 获取编码器支持的MIME类型，并进行匹配

                for (j in types.indices) {
                    if (types[j] == "video/avc") {
                        codecInfo = info
                        break
                    }
                }
            }

            if (codecInfo != null) {
                break
            }

            i++
        }

        val capabilities: MediaCodecInfo.CodecCapabilities? = codecInfo?.getCapabilitiesForType("video/avc")

        if (Build.VERSION.SDK_INT > 21 && capabilities != null) {
            val encoderCapabilities = capabilities.encoderCapabilities
            when {
                encoderCapabilities.isBitrateModeSupported(MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CQ) ->
                    bitRateMode = MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CQ
                encoderCapabilities.isBitrateModeSupported(MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CBR) ->
                    bitRateMode = MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CBR
                encoderCapabilities.isBitrateModeSupported(MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_VBR) ->
                    bitRateMode = MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_VBR
            }
        }

        if (capabilities != null) {
            for (i in capabilities.colorFormats.indices) {
                when (capabilities.colorFormats[i]) {
                    MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar,
                    MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar ->
                        colorFormat = capabilities.colorFormats[i]
                    else -> {
                    }
                }

                if (colorFormat != 0) {
                    break
                }
            }
        }

        movieRecorderAddress = nativeRecorderInit(savepath, width, height, bitRate, outputOrientation, colorFormat, bitRateMode)
    }


    override fun nativeOutput(): Long {
        return movieRecorderAddress
    }

    fun startRecorder(){
        nativeStartRecorder(movieRecorderAddress)
    }

    fun finishRecorder(){
        nativeFinishRecorder(movieRecorderAddress)
    }

    fun pauseRecorder(){
        nativePauseRecorder(movieRecorderAddress)
    }

    fun cancelRecording(){
        nativecancelRecording(movieRecorderAddress)
    }

    fun onSurfaceDestory(){
        nativeSurfaceDestory(movieRecorderAddress)
    }

    /**
     * TODO 这里还要写几个生命周期函数供JNI层反射回调
     */

    private external fun nativeRecorderInit(savepath: String, width: Int, height: Int, bitRate: Long,
                                            outputOrientation: Int, mediacodecColorFormat: Int, bitRateMode:Int): Long
    private external fun nativeStartRecorder(movieRecorderAddress: Long)
    private external fun nativeFinishRecorder(movieRecorderAddress: Long)
    private external fun nativePauseRecorder(movieRecorderAddress: Long)
    private external fun nativecancelRecording(movieWriterAddress: Long)

    private external fun nativeSurfaceDestory(movieWriterAddress: Long)

}