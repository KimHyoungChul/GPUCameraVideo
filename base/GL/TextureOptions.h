//
// Created by 刘轩 on 2019/1/8.
//

#ifndef GPUCAMERAVIDEO_TEXTUREOPTIONS_H
#define GPUCAMERAVIDEO_TEXTUREOPTIONS_H

#include <GLES2/gl2.h>

namespace GCVBase{

    typedef struct OptionTexture {
        GLenum minFilter = GL_LINEAR;
        GLenum maxFilter = GL_LINEAR;
        GLenum wrapS = GL_CLAMP_TO_EDGE;
        GLenum wrapT = GL_CLAMP_TO_EDGE;
        GLenum internalFormat = GL_RGBA;
        GLenum format = GL_RGBA;
        GLenum type = GL_UNSIGNED_BYTE;

        void bindTexture(GLenum target, GLuint texture){
            glBindTexture(target, texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        }

    } TextureOptions;
}

#endif //GPUCAMERAVIDEO_TEXTUREOPTIONS_H
