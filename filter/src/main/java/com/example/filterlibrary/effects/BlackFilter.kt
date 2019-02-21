package com.example.filterlibrary.effects

import com.example.filterlibrary.Filter


/**
 * Created by liuxuan on 2019/1/29
 */

class BlackFilter constructor(with: Int = 0, height: Int = 0) : Filter(with, height) {

    companion object {

        private val kFilterVertex: String = "attribute vec4 aPosition;\n" +
                "attribute vec4 aTexCoord;\n" +
                "varying vec2 vTexCoord;\n" +
                "void main() {\n"           +
                "   gl_Position = aPosition;\n" +
                "   vTexCoord = aTexCoord.xy;\n" +
                "}\n"

        private val kBlackFilterFragment: String = "varying highp vec2 vTexCoord;\n" +
                "uniform sampler2D uTexture;\n" +
                "void main() {\n" +
                "    vec4 vCameraColor  = texture2D(uTexture, vTexCoord);\n" +
                "    float fGrayColor = (0.3*vCameraColor.r + 0.59*vCameraColor.g + 0.11*vCameraColor.b);\n" +
                "    gl_FragColor = vec4(fGrayColor, fGrayColor, fGrayColor, 1.0);\n" +
                "}"
    }


    override fun getVertexShader(): String {
        return kFilterVertex
    }

    override fun getFragmentShader(): String {
        return kBlackFilterFragment
    }

}