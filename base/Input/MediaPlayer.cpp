//
// Created by 刘轩 on 2019/2/5.
//

#include "MediaBuffer.hpp"
#include "MediaPlayer.h"

GCVBase::MediaPlayer::MediaPlayer(std::string readPath, ANativeWindow * nativeWindow) {
    mediaDecoder = new MediaDecoder(readPath, nativeWindow);
}

GCVBase::MediaPlayer::~MediaPlayer() {
    delete mediaDecoder;
}

void GCVBase::MediaPlayer::startPlayer(const std::function<void ()> &handler) {
    mIsPlaying = true;
    startCallback = handler;

    while(mIsPlaying && !mediaDecoder->decodingIsFinished()){
        if(mediaDecoder->decodIsSurfacePlayer()){
            mediaDecoder->getCodecFrameVideoBuffer();
        } else {
            readNextVideoFrame();
        }

//        readNextAudioFrame();
    }
}

void GCVBase::MediaPlayer::pausePlayer(const std::function<void ()> &handler) {
    mIsPlaying = false;
    pauseCallback = handler;
}

void GCVBase::MediaPlayer::cancelPlayer(const std::function<void ()> &handler) {
    mIsPlaying = false;
    cancelCallback = handler;
}

void GCVBase::MediaPlayer::readNextVideoFrame() {

    runSyncContextLooper(Context::getShareContext()->getContextLooper(), [=] {

        MediaBuffer<FrameBuffer * > * videoBuffer = mediaDecoder->decodeNewFrameVideo();

        if (videoBuffer) {
            FrameBuffer *resultFbo = videoBuffer->mediaData;
            int rotation = (int) videoBuffer->metaData[RotationKey];

            RotationMode rotationMode = rotation0;
            switch (rotation) {
                case 90:
                    rotationMode = rotation0;
                    break;
                case 180:
                    rotationMode = rotation90;
                    break;
                case -90:
                    rotationMode = rotation270;
                    break;
                default:
                    break;
            }

            rotationPlayer = Rotation(rotationMode, FacingMode::FacingBack);
            newFrameReadyAtTime(rotationPlayer, resultFbo, videoBuffer->time);
        }
    });
}

void GCVBase::MediaPlayer::readNextAudioFrame() {
    if(mIsPlaying){
        MediaBuffer<uint8_t * > * audioBuffer = mediaDecoder->decodeNewFrameAudio();
    }

}

void GCVBase::MediaPlayer::newFrameReadyAtTime(const Rotation &mRotation, FrameBuffer *resultFbo, const MediaTime &time) {

    for(auto i = mTargets.begin(); i < mTargets.end(); i++){
        auto currentTarget =  * i;
        currentTarget->_setOutputRotation(mRotation);
        currentTarget->_setOutputFramebuffer(resultFbo);
//        currentTarget->_newFrameReadyAtTime(time);
    }
}

void GCVBase::MediaPlayer::setFilePath(std::string readPath) {
    mediaDecoder->setFilePath(readPath);
}

void GCVBase::MediaPlayer::setNativeSurface(ANativeWindow *nativeWindow) {
    mediaDecoder->setNativeSurface(nativeWindow);
}

