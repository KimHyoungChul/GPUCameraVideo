package com.example.codeclibrary

import android.media.MediaCodecInfo
import android.media.MediaCodecList
import android.os.Build
import android.util.Log
import com.example.baselib.GCVOutput

/**
 * Created by liuxuan on 2019/1/10
 */

class MovieRecorder(savepath: String, width: Int, height: Int, bitRate: Long, outputOrientation: Int) : GCVOutput{

    var movieRecorderAddress: Long = 0L

    init{
//        var colorFormat: Int = 0
//        var bitRateMode: Int = -1
//
//        val numCodecs = MediaCodecList.getCodecCount()
//        var codecInfo: MediaCodecInfo? = null
//
//        run {
//            var i = 0
//            while (i < numCodecs && codecInfo == null) {
//                val info: MediaCodecInfo = MediaCodecList.getCodecInfoAt(i)
//                if (!info.isEncoder) {
//                    continue
//                }
//
//                val types = info.supportedTypes
//
//                for (j in types.indices) {
//                    if (types[i] == "video/avc") {
//                        codecInfo = info
//                        break
//                    }
//                }
//
//                if (codecInfo != null) {
//                    break
//                }
//
//                i++
//            }
//        }
//
//        val capabilities: MediaCodecInfo.CodecCapabilities? = codecInfo?.getCapabilitiesForType("video/avc")
//
//        if (Build.VERSION.SDK_INT > 21 && capabilities != null) {
//            val encoderCapabilities = capabilities.encoderCapabilities
//            when {
//                encoderCapabilities.isBitrateModeSupported(MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CQ) ->
//                    bitRateMode = MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CQ
//                encoderCapabilities.isBitrateModeSupported(MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CBR) ->
//                    bitRateMode = MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_VBR
//                encoderCapabilities.isBitrateModeSupported(MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_VBR) ->
//                    bitRateMode = MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CBR
//            }
//        }
//
//        if (capabilities != null) {
//            for (i in capabilities.colorFormats.indices) {
//                when (capabilities.colorFormats[i]) {
//                    MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar,
//                    MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar ->
//                        colorFormat = capabilities.colorFormats[i]
//                    else -> {
//                    }
//                }
//
//                if (colorFormat != 0) {
//                    break
//                }
//            }
//        }
//        Log.e("colorFormat", colorFormat.toString() + "")
//        Log.e("bitRateMode", bitRateMode.toString() + "")
//        movieRecorderAddress = nativeRecorderInit(savepath, width, height, bitRate, outputOrientation, colorFormat, bitRateMode)
        movieRecorderAddress = nativeRecorderInit(savepath, width, height, bitRate, outputOrientation, 21, 2)

    }


    override fun nativeOutput(): Long {
        return movieRecorderAddress
    }

    fun startRecorder(){
        nativeStartRecorder(movieRecorderAddress)
    }

    fun FinishRecorder(){
        nativeFinishRecorder(movieRecorderAddress)
    }

    fun pauseRecorder(){
        nativePauseRecorder(movieRecorderAddress)
    }

    fun cancelRecording(){
        nativecancelRecording(movieRecorderAddress)
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

}