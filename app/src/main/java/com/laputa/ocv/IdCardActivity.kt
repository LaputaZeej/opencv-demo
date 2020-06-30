package com.laputa.ocv

import android.graphics.Bitmap
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.ImageView
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import com.laputa.ocv.idcard.IdCardCutOut
import com.laputa.ocv.idcard.IdCardRecognize
import com.laputa.ocv.selector.GlideEngine
import com.laputa.ocv.selector.toBitmap
import com.luck.picture.lib.PictureSelector
import com.luck.picture.lib.config.PictureMimeType
import com.luck.picture.lib.entity.LocalMedia
import com.luck.picture.lib.listener.OnResultCallbackListener
import kotlinx.android.synthetic.main.activity_id_card.*

class IdCardActivity : AppCompatActivity() {
    lateinit var viewModel: IdCardViewModel
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_id_card)
        viewModel = ViewModelProvider(this)[IdCardViewModel::class.java].apply {
            originalIdCard.observe(this@IdCardActivity, Observer {
                iv_01.updateBitmap(it)
            })

            resizeIdCard.observe(this@IdCardActivity, Observer {
                iv_02.updateBitmap(it)
            })

            cvtColorIdCard.observe(this@IdCardActivity, Observer {
                iv_03.updateBitmap(it)
            })

            thresholdIdCard.observe(this@IdCardActivity, Observer {
                iv_04.updateBitmap(it)
            })

            erodeIdCard.observe(this@IdCardActivity, Observer {
                iv_05.updateBitmap(it)
            })
            contoursIdCard.observe(this@IdCardActivity, Observer {
                iv_06.updateBitmap(it)
            })
            filterContoursIdCard.observe(this@IdCardActivity, Observer {
                iv_07.updateBitmap(it)
            })

            cutOutIdCard.observe(this@IdCardActivity, Observer {
                iv_08.updateBitmap(it)
            })

            result.observe(this@IdCardActivity, Observer {
                updateInfo {
                    it ?: " empty "
                }
            })
        }

        iv_01.setOnClickListener {
            select()
        }

        iv_02.setOnClickListener {
            viewModel.resize()
        }

        iv_03.setOnClickListener {
            viewModel.cvtColor()
        }

        iv_04.setOnClickListener {
            viewModel.threshold()
        }

        iv_05.setOnClickListener {
            viewModel.erode()
        }

        iv_06.setOnClickListener {
            viewModel.contours()
        }

        iv_07.setOnClickListener {
            viewModel.filterContours()
        }

        iv_08.setOnClickListener {
            viewModel.cutOutIdCard()
        }
        tv_card_recognize.setOnClickListener {
            viewModel.recognizeCardId()
        }
        btn_clear.setOnClickListener {
            viewModel.clear()
        }
    }

    private fun ImageView.updateBitmap(it: Bitmap?) {
        it?.let { bitmap ->
            setImageBitmap(bitmap)
        } ?: kotlin.run {
            setImageResource(R.mipmap.ic_launcher)
        }
    }

    private fun select() {
        PictureSelector.create(this).openGallery(PictureMimeType.ofAll())
            .loadImageEngine(GlideEngine.createGlideEngine())
            .forResult(object : OnResultCallbackListener<LocalMedia> {
                override fun onResult(result: MutableList<LocalMedia>?) {
                    when {
                        result.isNullOrEmpty() -> {

                        }
                        else -> {
                            result[0].run {
                                updateInfo {
                                    "${this.originalPath} \n ${path}"
                                }
                                viewModel.originalIdCard(this)
                            }
                        }
                    }
                }

                override fun onCancel() {
                }
            })
    }


    private inline fun updateInfo(action: () -> String) {
        tv_card_id.text = action()
    }


    class IdCardViewModel : ViewModel() {
        private val _originalIdCard: MutableLiveData<Bitmap> = MutableLiveData()
        private val _resizeIdCard: MutableLiveData<Bitmap> = MutableLiveData()
        private val _cvtColorIdCard: MutableLiveData<Bitmap> = MutableLiveData()
        private val _thresholdIdCard: MutableLiveData<Bitmap> = MutableLiveData()
        private val _erodeIdCard: MutableLiveData<Bitmap> = MutableLiveData()
        private val _contoursIdCard: MutableLiveData<Bitmap> = MutableLiveData()
        private val _filterContoursIdCard: MutableLiveData<Bitmap> = MutableLiveData()
        private val _cutOutIdCard: MutableLiveData<Bitmap> = MutableLiveData()
        private val _result: MutableLiveData<String> = MutableLiveData()
        val originalIdCard: LiveData<Bitmap> = _originalIdCard
        val resizeIdCard: LiveData<Bitmap> = _resizeIdCard
        val cvtColorIdCard: LiveData<Bitmap> = _cvtColorIdCard
        val thresholdIdCard: LiveData<Bitmap> = _thresholdIdCard
        val erodeIdCard: LiveData<Bitmap> = _erodeIdCard
        val contoursIdCard: LiveData<Bitmap> = _contoursIdCard
        val filterContoursIdCard: LiveData<Bitmap> = _filterContoursIdCard
        val cutOutIdCard: LiveData<Bitmap> = _cutOutIdCard
        val result: LiveData<String> = _result

        init {
            IdCardRecognize.initTrainedData()
        }

        fun originalIdCard(localMedia: LocalMedia) {
            _originalIdCard.value = toBitmap(localMedia.path)

            resize()
            cvtColor()
            threshold()
            erode()
            threshold()
            contours()
            filterContours()
            cutOutIdCard()
        }

        fun resize() {
            _resizeIdCard.value = _originalIdCard.value?.run {
                IdCardCutOut.resize(this, Bitmap.Config.ARGB_8888)
            }
        }

        /**
         * 灰度化
         */
        fun cvtColor() {
            _cvtColorIdCard.value = _originalIdCard.value?.run {
                IdCardCutOut.cvtColor(this, Bitmap.Config.ARGB_8888)
            }
        }

        /**
         * 二值化（黑白）
         */
        fun threshold() {
            _thresholdIdCard.value.gc()
            _thresholdIdCard.value = _originalIdCard.value?.run {
                IdCardCutOut.threshold(this, Bitmap.Config.ARGB_8888)
            }
        }

        /**
         * 腐蚀（虚化）
         */
        fun erode() {
            _erodeIdCard.value.gc()
            _erodeIdCard.value = _originalIdCard.value?.run {
                IdCardCutOut.erode(this, Bitmap.Config.ARGB_8888)
            }
        }

        /**
         * 轮廓检测
         */
        fun contours() {
            _contoursIdCard.value.gc()
            _contoursIdCard.value = _originalIdCard.value?.run {
                IdCardCutOut.contours(this, Bitmap.Config.ARGB_8888)
            }
        }

        /**
         * 过滤
         */
        fun filterContours() {
            _filterContoursIdCard.value.gc()
            _filterContoursIdCard.value = _originalIdCard.value?.run {
                IdCardCutOut.filterContours(this, Bitmap.Config.ARGB_8888)
            }
        }

        /**
         * 截取身份证编号
         */
        fun cutOutIdCard() {
            _cutOutIdCard.value.gc()
            _cutOutIdCard.value = _originalIdCard.value?.run {
                IdCardCutOut.cutOutIdCard(this, Bitmap.Config.ARGB_8888)
            }
        }

        fun recognizeCardId() {
            _result.value = _cutOutIdCard.value?.let {
                IdCardRecognize.recognize(it)
            }
        }

        fun clear() {
            _originalIdCard.value.gc()
            _resizeIdCard.value.gc()
            _cvtColorIdCard.value.gc()
            _thresholdIdCard.value.gc()
            _erodeIdCard.value.gc()
            _contoursIdCard.value.gc()
            _filterContoursIdCard.value.gc()
            _cutOutIdCard.value.gc()

            _originalIdCard.value = null
            _resizeIdCard.value = null
            _cvtColorIdCard.value = null
            _thresholdIdCard.value = null
            _erodeIdCard.value = null
            _contoursIdCard.value = null
            _filterContoursIdCard.value = null
            _cutOutIdCard.value = null
            _result.value = null
        }


        private fun Bitmap?.gc() {
            this?.recycle()
        }

        // 得到一个新的Bitmap
        // 得到新的bitmap将在C++里进行变换，每一次操作前新建一个bitmap，否则是同一个bitmap
        // @deprecated
        private fun Bitmap.cpy(): Bitmap = this.copy(Bitmap.Config.ARGB_8888, false)

        override fun onCleared() {
            super.onCleared()
            _originalIdCard.value.gc()
            _resizeIdCard.value.gc()
            _cvtColorIdCard.value.gc()
            _thresholdIdCard.value.gc()
            _erodeIdCard.value.gc()
            _contoursIdCard.value.gc()
            _filterContoursIdCard.value.gc()
            _cutOutIdCard.value.gc()
        }
    }
}