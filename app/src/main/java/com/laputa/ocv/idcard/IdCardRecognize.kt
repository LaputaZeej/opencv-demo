package com.laputa.ocv.idcard

import android.graphics.Bitmap
import android.os.Environment
import android.util.Log
import com.googlecode.tesseract.android.TessBaseAPI

/**
 * 识别
 */
object IdCardRecognize {

    private val tessBase: TessBaseAPI by lazy { TessBaseAPI() }

    private val data_path = Environment.getExternalStorageDirectory().path + "/tess"

    /**
     * 加载样本
     */
    fun initTrainedData() {
        // 复制样本到根目录,这里直接手动复制了
        copy {
            tessBase.init(it, "zh");
        }
    }

    fun recognize(bitmap: Bitmap): String? = tessBase.also { it.setImage(bitmap) }.utF8Text

    private inline fun copy(complete: (String) -> Unit) {
        Log.i("zeej", "path = " + data_path)
        complete(data_path)
    }

}