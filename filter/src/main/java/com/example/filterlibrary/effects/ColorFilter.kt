package com.example.filterlibrary.effects

import com.example.filterlibrary.Filter
import java.util.logging.LogRecord

/**
 * Created by liuxuan on 2019/1/29
 */

class ColorFilter : Filter() {

    companion object {

        private val kFilterVertex: String = "attribute vec4 aPosition;\n" +
                "attribute vec4 aTexCoord;\n" +
                "varying vec2 vTexCoord;\n" +
                "void main() {\n"           +
                "   gl_Position = aPosition;\n" +
                "   vTexCoord = aTexCoord.xy;\n" +
                "}\n"

        private val kColorFilterFragment: String = "varying highp vec2 vTexCoord;\n" +
                "uniform sampler2D uTexture;\n" +
                "void main() {\n" +
                "    vec4 color  = texture2D(uTexture, vTexCoord);\n" +
                "    gl_FragColor = vec4(1.0 -color.rgb, color.a);" +
                "}"
    }

    override fun getVertexShader(): String {
        return kFilterVertex
    }

    override fun getFragmentShader(): String {
        return kColorFilterFragment
    }

}