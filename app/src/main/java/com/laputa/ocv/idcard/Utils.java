package com.laputa.ocv.idcard;

import android.graphics.Bitmap;
import android.util.Log;

import org.opencv.core.Mat;

public class Utils {
    public static void logi(String tag, String msg) {
        if (tag == null || msg == null) throw new NullPointerException("tag or msg is null");
        Log.i(tag, msg);
    }

    public static void mat2Bitmap(Mat mat, Bitmap bitmap) {
        logi("zeej", "[java] mat2Bitmap mat = " + mat + ",bitmap = " + bitmap);
        org.opencv.android.Utils.matToBitmap(mat, bitmap);
    }

    public static void bitmap2Mat(Bitmap bitmap, Mat mat) {
        logi("zeej", "[java] mat2Bitmap mat = " + mat + ",bitmap = " + bitmap);
        org.opencv.android.Utils.bitmapToMat(bitmap, mat);
    }
}
