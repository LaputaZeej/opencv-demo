package com.laputa.ocv.idcard

import android.graphics.Bitmap

/**
 * 截取
 */
class IdCardCutOut {
    companion object{
        /**
         * 压缩
         */
        external  fun resize(src:Bitmap,config:Bitmap.Config):Bitmap

        /**
         * 灰度化
         */
        external  fun cvtColor(src:Bitmap,config:Bitmap.Config):Bitmap

        /**
         * 二值化（黑白）
         */
        external  fun threshold(src:Bitmap,config:Bitmap.Config):Bitmap

        /**
         * 腐蚀（虚化）
         */
        external  fun erode(src:Bitmap,config:Bitmap.Config):Bitmap

        /**
         * 轮廓检测
         */
        external  fun contours(src:Bitmap,config:Bitmap.Config):Bitmap

        /**
         * 过滤
         */
        external  fun filterContours(src:Bitmap,config:Bitmap.Config):Bitmap

        /**
         * 截取身份证编号
         */
        external  fun cutOutIdCard(src:Bitmap,config:Bitmap.Config):Bitmap
    }
}