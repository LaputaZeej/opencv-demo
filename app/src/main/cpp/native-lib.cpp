#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <opencv2/opencv.hpp>
#include "bitmap.h"

//#include "include/opencv/cv.h"
//#include "include/opencv2/opencv.hpp"

#define ANDROID_TAG  "zeej"
#define DEFAULT_CARD_WIDTH 640
#define DEFAULT_CARD_HEIGHT 400
#define  FIX_ID_CARD_SIZE Size(DEFAULT_CARD_WIDTH,DEFAULT_CARD_HEIGHT)
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, ANDROID_TAG, __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, ANDROID_TAG, __VA_ARGS__))

using namespace std;
using namespace cv;


// ############################################### Bitmap Mat #################################################################  //

void BitmapToMat2(JNIEnv *env, jobject &bitmap, Mat &mat, jboolean needUnPremultiplyAlpha) {
    AndroidBitmapInfo info;
    void *pixels = 0;
    Mat &dst = mat;

    try {
        LOGD("nBitmapToMat");
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        dst.create(info.height, info.width, CV_8UC4);
        if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            LOGD("nBitmapToMat: RGBA_8888 -> CV_8UC4");
            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if (needUnPremultiplyAlpha) cvtColor(tmp, dst, COLOR_mRGBA2RGBA);
            else tmp.copyTo(dst);
        } else {
            // info.format == ANDROID_BITMAP_FORMAT_RGB_565
            LOGD("nBitmapToMat: RGB_565 -> CV_8UC4");
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            cvtColor(tmp, dst, COLOR_BGR5652RGBA);
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return;
    } catch (const cv::Exception &e) {
        AndroidBitmap_unlockPixels(env, bitmap);
        LOGE("nBitmapToMat catched cv::Exception: %s", e.what());
        jclass je = env->FindClass("org/opencv/core/CvException");
        if (!je) je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        LOGE("nBitmapToMat catched unknown exception (...)");
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nBitmapToMat}");
        return;
    }
}

void BitmapToMat(JNIEnv *env, jobject &bitmap, Mat &mat) {
    BitmapToMat2(env, bitmap, mat, false);
}

void MatToBitmap2
        (JNIEnv *env, Mat &mat, jobject &bitmap, jboolean needPremultiplyAlpha) {
    AndroidBitmapInfo info;
    void *pixels = 0;
    Mat &src = mat;

    try {
        LOGD("nMatToBitmap");
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        // 以下的作用?打开会报错- -
//        CV_Assert(src.dims == 2 && info.height == (uint32_t) src.rows &&
//                  info.width == (uint32_t) src.cols);
        CV_Assert(src.type() == CV_8UC1 || src.type() == CV_8UC3 || src.type() == CV_8UC4);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if (src.type() == CV_8UC1) {
                LOGD("nMatToBitmap: CV_8UC1 -> RGBA_8888");
                cvtColor(src, tmp, COLOR_GRAY2RGBA);
            } else if (src.type() == CV_8UC3) {
                LOGD("nMatToBitmap: CV_8UC3 -> RGBA_8888");
                cvtColor(src, tmp, COLOR_RGB2RGBA);
            } else if (src.type() == CV_8UC4) {
                LOGD("nMatToBitmap: CV_8UC4 -> RGBA_8888");
                if (needPremultiplyAlpha)
                    cvtColor(src, tmp, COLOR_RGBA2mRGBA);
                else
                    src.copyTo(tmp);
            }
        } else {
            // info.format == ANDROID_BITMAP_FORMAT_RGB_565
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            if (src.type() == CV_8UC1) {
                LOGD("nMatToBitmap: CV_8UC1 -> RGB_565");
                cvtColor(src, tmp, COLOR_GRAY2BGR565);
            } else if (src.type() == CV_8UC3) {
                LOGD("nMatToBitmap: CV_8UC3 -> RGB_565");
                cvtColor(src, tmp, COLOR_RGB2BGR565);
            } else if (src.type() == CV_8UC4) {
                LOGD("nMatToBitmap: CV_8UC4 -> RGB_565");
                cvtColor(src, tmp, COLOR_RGBA2BGR565);
            }
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return;
    } catch (const cv::Exception &e) {
        AndroidBitmap_unlockPixels(env, bitmap);
        LOGE("nMatToBitmap catched cv::Exception: %s", e.what());
        jclass je = env->FindClass("org/opencv/core/CvException");
        if (!je) je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        LOGE("nMatToBitmap catched unknown exception (...)");
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nMatToBitmap}");
        return;
    }
}

void MatToBitmap(JNIEnv *env, Mat &mat, jobject &bitmap) {
    MatToBitmap2(env, mat, bitmap, false);
}

// ############################################### Bitmap Mat #################################################################  //

// ############################################### 测试JNI #################################################################  //
extern "C" JNIEXPORT jstring JNICALL
Java_com_laputa_ocv_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
// ############################################### 测试JNI #################################################################  //

// ############################################### 创建bitmap #################################################################  //
jobject createBitmap(JNIEnv *env, Mat srcData, jobject config) {
    // Image Details
    int imgWidth = srcData.cols;
    int imgHeight = srcData.rows;
    int numPix = imgWidth * imgHeight;
    jclass bmpCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapMid = env->GetStaticMethodID(bmpCls, "createBitmap",
                                                       "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jobject jBmpObj = env->CallStaticObjectMethod(bmpCls, createBitmapMid, imgWidth, imgHeight,
                                                  config);
    MatToBitmap(env, srcData, jBmpObj);
    return jBmpObj;
}
// ############################################### 创建bitmap #################################################################  //

// ############################################### 身份证截取 #################################################################  //

/**
 * 压缩
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_laputa_ocv_idcard_IdCardCutOut_00024Companion_resize(JNIEnv *env, jobject thiz,
                                                              jobject src, jobject config) {
    LOGE("测试JNI");
    // 测试jni调用java
    jclass logsClass = env->FindClass("com/laputa/ocv/idcard/Utils");
    if (logsClass == nullptr) {
        return src;
    }
    jmethodID logiMethodId = env->GetStaticMethodID(logsClass, "logi",
                                                    "(Ljava/lang/String;Ljava/lang/String;)V");
    if (logiMethodId == nullptr) {
        return src;
    }
    string hello = "调用java静态方法";
    string tag = ANDROID_TAG;
    jstring hello_jsting = env->NewStringUTF(hello.c_str());
    jstring tag_jsting = env->NewStringUTF(tag.c_str());
    env->CallStaticVoidMethod(logsClass, logiMethodId, tag_jsting, hello_jsting);

    // com/laputa/ocv/idcard/Logs
    jclass logsClassK = env->FindClass("com/laputa/ocv/idcard/Logs");
    __android_log_print(ANDROID_LOG_INFO, ANDROID_TAG, "图片处理 resize 0001");
    if (logsClassK == nullptr) {
        return src;
    }
    __android_log_print(ANDROID_LOG_ERROR, ANDROID_TAG, "图片处理 resize 0002");
    jmethodID logiMethodIdK = env->GetStaticMethodID(logsClassK, "logi",
                                                     "(Ljava/lang/String;Ljava/lang/String;)V");
    __android_log_print(ANDROID_LOG_ERROR, ANDROID_TAG, "图片处理 resize 0003");
    if (logiMethodIdK == nullptr) {
        return src;
    }
    __android_log_print(ANDROID_LOG_ERROR, ANDROID_TAG, "图片处理 resize 0004");
    env->CallStaticVoidMethod(logsClassK, logiMethodIdK, tag_jsting, hello_jsting);
    __android_log_print(ANDROID_LOG_ERROR, ANDROID_TAG, "图片处理 resize 0005");
    // 测试jni调用java end

    LOGE("压缩");
    // resize
    Mat mat_src;
    Mat mat_dst;
    __android_log_print(ANDROID_LOG_ERROR, ANDROID_TAG, "图片处理 resize 开始");
    BitmapToMat(env, src, mat_src);
    __android_log_print(ANDROID_LOG_ERROR, ANDROID_TAG, "图片处理 resize 转成Mat %d", mat_src.cols);
    __android_log_print(ANDROID_LOG_ERROR, ANDROID_TAG, "图片处理 resize 转成Mat %d", mat_src.rows);
    resize(mat_src, mat_dst, FIX_ID_CARD_SIZE);
    __android_log_print(ANDROID_LOG_ERROR, ANDROID_TAG, "图片处理 resize 目标 %d", mat_dst.cols);
    __android_log_print(ANDROID_LOG_ERROR, ANDROID_TAG, "图片处理 resize 目标 %d", mat_dst.rows);
    jobject resize_bitmap = createBitmap(env, mat_dst, config);
    __android_log_print(ANDROID_LOG_ERROR, ANDROID_TAG, "图片处理 resize 结束");
    mat_src.release();
    mat_dst.release();
    return resize_bitmap;
}

/**
 * 压缩+灰度
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_laputa_ocv_idcard_IdCardCutOut_00024Companion_cvtColor(JNIEnv *env, jobject thiz,
                                                                jobject src, jobject config) {
    Mat mat_src;
    Mat mat_dst;
    BitmapToMat(env, src, mat_src);
    resize(mat_src, mat_dst, FIX_ID_CARD_SIZE);
    cvtColor(mat_dst, mat_dst, COLOR_BGR2GRAY);
    jobject cvt_bitmap = createBitmap(env, mat_dst, config);
    mat_src.release();
    mat_dst.release();
    return cvt_bitmap;
}

/**
 * 压缩+灰度+二值化
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_laputa_ocv_idcard_IdCardCutOut_00024Companion_threshold(JNIEnv *env, jobject thiz,
                                                                 jobject src, jobject config) {
    Mat mat_src;
    Mat mat_dst;
    BitmapToMat(env, src, mat_src);
    resize(mat_src, mat_dst, FIX_ID_CARD_SIZE);
    cvtColor(mat_dst, mat_dst, COLOR_BGR2GRAY);
    threshold(mat_dst, mat_dst, 100, 255, THRESH_BINARY);
    jobject threshold_bitmap = createBitmap(env, mat_dst, config);
    mat_src.release();
    mat_dst.release();
    return threshold_bitmap;
}

/**
 * 压缩+灰度+二值化+腐蚀
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_laputa_ocv_idcard_IdCardCutOut_00024Companion_erode(JNIEnv *env, jobject thiz, jobject src,
                                                             jobject config) {
    Mat mat_src;
    Mat mat_dst;
    BitmapToMat(env, src, mat_src);
    resize(mat_src, mat_dst, FIX_ID_CARD_SIZE);
    cvtColor(mat_dst, mat_dst, COLOR_BGR2GRAY);
    threshold(mat_dst, mat_dst, 100, 255, THRESH_BINARY);
    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(15, 10));
    erode(mat_dst, mat_dst, erodeElement);
    jobject ercode_bitmap = createBitmap(env, mat_dst, config);
    mat_src.release();
    mat_dst.release();
    return ercode_bitmap;
}

/**
 * 压缩+灰度+二值化+腐蚀+轮廓检测
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_laputa_ocv_idcard_IdCardCutOut_00024Companion_contours(JNIEnv *env, jobject thiz,
                                                                jobject src, jobject config) {
    Mat mat_src;
    Mat mat_dst;
    BitmapToMat(env, src, mat_src);
    resize(mat_src, mat_dst, FIX_ID_CARD_SIZE);
    cvtColor(mat_dst, mat_dst, COLOR_BGR2GRAY);
    threshold(mat_dst, mat_dst, 100, 255, THRESH_BINARY);
    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(15, 10));
    erode(mat_dst, mat_dst, erodeElement);


    vector<vector<Point> > contours;
    vector<Rect> rects;

    findContours(mat_dst, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测...size = %d", contours.size());


    for (int i = 0; i < contours.size(); i++) {
        Rect rect = boundingRect(contours.at(i));
        LOGD("轮廓检测 %d",i);
        rectangle(mat_dst, rect, Scalar(0, 0, 255));
        if (rect.width > rect.height * 8) {
            __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 过滤");
            rects.push_back(rect);
            //rectangle(mat_dst, rect, Scalar(0, 255, 255));
        }
    }
    jobject countors_bitmap = createBitmap(env, mat_dst, config);
    mat_src.release();
    mat_dst.release();
    return countors_bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_laputa_ocv_idcard_IdCardCutOut_00024Companion_filterContours(JNIEnv *env, jobject thiz,
                                                                      jobject src, jobject config) {
    Mat mat_src;
    Mat mat_dst;
    BitmapToMat(env, src, mat_src);
    resize(mat_src, mat_dst, FIX_ID_CARD_SIZE);
    cvtColor(mat_dst, mat_dst, COLOR_BGR2GRAY);
    threshold(mat_dst, mat_dst, 100, 255, THRESH_BINARY);
    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(15, 10));
    erode(mat_dst, mat_dst, erodeElement);


    vector<vector<Point> > contours;
    vector<Rect> rects;

    findContours(mat_dst, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 所有轮廓数量:%d", contours.size());

    for (int i = 0; i < contours.size(); i++) {
        Rect rect = boundingRect(contours.at(i));
        if (rect.width > rect.height * 8) {
            __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 过滤1:9");
            rects.push_back(rect);
        }
    }
    __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 过滤后1:9轮廓数量:%d", rects.size());
    Rect finalRect;
    if (rects.size() == 1) {
        finalRect = rects.at(0);
    } else {
        int lowPoint = 0;
        for (int i = 0; i < rects.size(); i++) {
            __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测->%d", i);
            Rect rect = rects.at(i);
            Point p = rect.tl();
            if (rect.tl().y > lowPoint) {
                lowPoint = rect.tl().y;
                finalRect = rect;
            }
        }
    }

    // 原图resize一下
    resize(mat_src, mat_src, FIX_ID_CARD_SIZE);
    rectangle(mat_src, finalRect, Scalar(255, 0, 0, 255), 2);
    __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 过滤完成 finalRect = (%d,%d,%d,%d)",
                        finalRect.x,
                        finalRect.y, finalRect.width, finalRect.height);
    jobject result = createBitmap(env, mat_src, config);
    mat_src.release();
    mat_dst.release();
    return result;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_laputa_ocv_idcard_IdCardCutOut_00024Companion_cutOutIdCard(JNIEnv *env, jobject thiz,
                                                                    jobject src, jobject config) {
    Mat src_img;
    Mat dst_img;
    BitmapToMat(env, src, src_img);
    Mat dst;
    resize(src_img, src_img, FIX_ID_CARD_SIZE);
    cvtColor(src_img, dst, COLOR_BGR2GRAY);
    threshold(dst, dst, 100, 255, THRESH_BINARY);
    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(15, 10));
    erode(dst, dst, erodeElement);
    vector<vector<Point> > contours;
    vector<Rect> rects;
    findContours(dst, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测...size = %d", contours.size());
    for (int i = 0; i < contours.size(); i++) {
        Rect rect = boundingRect(contours.at(i));
        rectangle(dst, rect, Scalar(0, 0, 255));  // 在dst 图片上显示 rect 矩形
        if (rect.width > rect.height * 8) {
            __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 过滤1:9");
            rects.push_back(rect);
            rectangle(dst, rect, Scalar(0, 255, 255));
            dst_img = src_img(rect);
        }

    }
    __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 rect size = %d", rects.size());
    if (rects.size() == 1) {
        Rect rect = rects.at(0);
        dst_img = src_img(rect);
    } else {
        int lowPoint = 0;
        Rect finalRect;
        for (int i = 0; i < rects.size(); i++) {
            Rect rect = rects.at(i);
            Point p = rect.tl();
            if (rect.tl().y > lowPoint) {
                __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 过滤成最底rect");
                lowPoint = rect.tl().y;
                finalRect = rect;
            }
        }

        __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 finalRect");
        rectangle(dst, finalRect, Scalar(255, 255, 0));
        //imshow("contours", dst);
        dst_img = src_img(finalRect);
    }
    __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 dst_img rows=%d", dst_img.rows);
    __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 dst_img cols=%d", dst_img.cols);
    jobject result = createBitmap(env, dst_img, config);
    src_img.release();
    dst_img.release();
    dst.release();
    return result;

//    Mat mat_src;
//    Mat mat_dst;
//    BitmapToMat(env, src, mat_src);
//    // 无损压缩//640*400
//    resize(mat_src, mat_dst, FIX_ID_CARD_SIZE);
//    // 灰度化
//    cvtColor(mat_src, mat_dst, COLOR_BGR2GRAY);
//    // 二值化
//    threshold(mat_dst, mat_dst, 100, 255, THRESH_BINARY);
//    // 腐蚀
//    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(15, 10));
//    erode(mat_dst, mat_dst, erodeElement);
//
//    // 轮廓检测
//    vector<vector<Point> > contours;
//    vector<Rect> rects;
//    findContours(mat_dst, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
//
//    // 过滤
//    Mat dst;
//    __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测...一共size = %d",contours.size());
//    for (int i = 0; i < contours.size(); i++) {
//        Rect rect = boundingRect(contours.at(i));
//        rectangle(mat_dst, rect, Scalar(0, 0, 255));  // 在dst 图片上显示 rect 矩形
//        if (rect.width > rect.height * 7) {
//            __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 过滤1:9");
//            rects.push_back(rect);
//            rectangle(mat_dst, rect, Scalar(0, 255, 255));
//            dst = mat_src(rect);
//        }
//    }
//    __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测...过滤后size = %d",rects.size());
//    if (rects.size() == 1) {
//        Rect rect = rects.at(0);
//        dst = mat_src(rect);
//    } else {
//        int lowPoint = 0;
//        Rect finalRect;
//        for (int i = 0; i < rects.size(); i++) {
//            Rect rect = rects.at(i);
//            Point p = rect.tl();
//            __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 。。。。。%d vs %d",rect.tl().y ,lowPoint);
//            if (rect.tl().y > lowPoint) {
//                __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 过滤成最底rect");
//                lowPoint = rect.tl().y;
//                finalRect = rect;
//            }
//        }
//
//        __android_log_print(ANDROID_LOG_INFO, "zeej", "轮廓检测 finalRect %d",finalRect.tl().y);
//        rectangle(mat_dst, finalRect, Scalar(255, 255, 0));
//        //imshow("contours", dst);
//        dst = mat_src(finalRect);
//    }
//
//    // 生成图片
//    MatToBitmap(env, dst, src);
//
//    // 回收
//    mat_src.release();
//    mat_dst.release();
//    dst.release();
//    return src;
}

// ############################################### 身份证截取 #################################################################  //
