//
// Created by 刘轩 on 2019/2/5.
//

#ifndef GPUCAMERAVIDEO_MEDIAPLAYER_H
#define GPUCAMERAVIDEO_MEDIAPLAYER_H

#include "Decode/MediaDecoder.h"
#include "NativeInput.h"

namespace GCVBase {

    class MediaPlayer : NativeInput {

    public:
        MediaPlayer(std::string readPath = nullptr, ANativeWindow * nativeWindow = nullptr);
        ~MediaPlayer();

        void setFilePath( std::string readPath);

        void startPlayer(const std::function<void ()> &handler);
        void pausePlayer(const std::function<void ()> &handler);
        void cancelPlayer(const std::function<void ()> &handler);

        void setNativeSurface(ANativeWindow * nativeWindow);

    private:

        bool mIsPlaying = false;

        MediaDecoder * mediaDecoder = NULL;

        Rotation rotationPlayer = Rotation::defaultRotation();

        std::function<void ()> startCallback = NULL;
        std::function<void ()> pauseCallback = NULL;
        std::function<void ()> cancelCallback = NULL;

        void readNextVideoFrame();
        void readNextAudioFrame();

        void newFrameReadyAtTime(const GCVBase::Rotation &mRotation,FrameBuffer *resultFbo, const MediaTime &time);
    };
}


#endif //GPUCAMERAVIDEO_MEDIAPLAYER_H
