package com.laputa.ocv.selector

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.text.TextUtils

fun toBitmap(pathName: String?): Bitmap? {
    if (TextUtils.isEmpty(pathName)) return null
    val o = BitmapFactory.Options()
    o.inJustDecodeBounds = true
    BitmapFactory.decodeFile(pathName, o)
    var width_tmp = o.outWidth
    var height_tmp = o.outHeight
    var scale = 1
    while (true) {
        if (width_tmp <= 640 && height_tmp <= 480) {
            break
        }
        width_tmp /= 2
        height_tmp /= 2
        scale *= 2
    }
    val opts = BitmapFactory.Options()
    opts.inSampleSize = scale
    opts.outHeight = height_tmp
    opts.outWidth = width_tmp
    return BitmapFactory.decodeFile(pathName, opts)
}