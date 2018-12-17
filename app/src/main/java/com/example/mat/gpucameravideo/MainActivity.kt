package com.example.mat.gpucameravideo

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import com.example.codeclibrary.CodecMain
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Example of a call to a native method
        sample_text.text = CodecMain().getStringFromCodec()
    }
}
