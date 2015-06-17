////opencv
#include <opencv2/opencv.hpp>

#include "./video_decoder.h"

int VideoDecoder::Init(std::string video_path, std::string output_path, std::string tmp_file) {
    this->video_path_ = video_path;
    this->output_path_ = output_path;
    this->tmp_file_ = tmp_file;
	av_register_all();
	this->format_ctx_ = avformat_alloc_context();

	if (avformat_open_input(&this->format_ctx_, video_path.c_str(), NULL, NULL) < 0)
		return -1;

	if (avformat_find_stream_info(this->format_ctx_, NULL) < 0)
		return -2;

	this->video_stream_ = -1;

    for (size_t i = 0; i < format_ctx_->nb_streams; i++) {
        if (format_ctx_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_ = i;
            break;
        }
    }
	if (video_stream_ == -1)
		return -3;

	this->codec_ctx_ = format_ctx_->streams[video_stream_]->codec;
	auto codec_decoder_id = avcodec_find_decoder(codec_ctx_->codec_id);
	if (codec_decoder_id == NULL)
		return -4;
	if (avcodec_open2(codec_ctx_, codec_decoder_id, NULL) < 0)
		return -5;
	this->frame_ = av_frame_alloc();
	this->frame_RGB_ = av_frame_alloc();
	auto num_bytes = avpicture_get_size(PIX_FMT_RGB24, codec_ctx_->width, codec_ctx_->height);
	auto buffer = new uint8_t[num_bytes];
	avpicture_fill((AVPicture *)frame_RGB_, buffer, PIX_FMT_RGB24, codec_ctx_->width, codec_ctx_->height);
	this->sws_ctx_ = sws_getContext(codec_ctx_->width, codec_ctx_->height, codec_ctx_->pix_fmt, codec_ctx_->width, codec_ctx_->height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
    return 0;
}

int VideoDecoder::SetDecoderCallback(DecoderCallback callback) {
    this->callback_ = callback;
    return 0;
}

int VideoDecoder::StartDecode() {
	AVPacket packet;
    int is_frame_finished;
	while (av_read_frame(format_ctx_, &packet) >= 0) {
		if (packet.stream_index == video_stream_) {
			avcodec_decode_video2(codec_ctx_, frame_, &is_frame_finished, &packet);
			if (is_frame_finished) {
				int num = frame_->coded_picture_number;
				sws_scale(sws_ctx_, frame_->data, frame_->linesize, 0, codec_ctx_->height, frame_RGB_->data, frame_RGB_->linesize);
                //string tmp_file_name = tmp_file_ + "_" + std::to_string(num) + ".jpg";
                string tmp_file_name = tmp_file_ + ".jpg";
                if (!this->SaveFrameToJpeg(frame_RGB_, tmp_file_name)) {
                    callback_(std::to_string(num));
                }
			}
		}
		av_free_packet(&packet);
	}
    this->Release();
    return 0;
}

int VideoDecoder::Release() {
    if (this->frame_RGB_) {
        av_free(this->frame_RGB_);
        this->frame_RGB_ = nullptr;
    }
    if (this->frame_) {
        av_free(this->frame_);
        this->frame_ = nullptr;
    }
    if (this->sws_ctx_) {
        sws_freeContext(this->sws_ctx_);
        this->sws_ctx_ = nullptr;
    }
    if (this->codec_ctx_) {
        avcodec_close(this->codec_ctx_);
        this->codec_ctx_ = nullptr;
    }
    if (this->format_ctx_) {
        avformat_close_input(&this->format_ctx_);
        this->format_ctx_ = nullptr;
    }
    return 0;
}

int VideoDecoder::SaveFrameToJpeg(AVFrame* frame, std::string save_file_name) {
    cv::Mat mat(cv::Size(codec_ctx_->width, codec_ctx_->height), CV_8UC3, cv::Scalar(0));
    auto data_pointer = mat.data;
	auto step_width = mat.step;
	auto channel_size = mat.channels();
	for (int i = 0; i < codec_ctx_->height; i++) {
		for (int j = 0; j < codec_ctx_->width; j++) {
			data_pointer[i*step_width + j*channel_size + 0] = frame->data[0][i*frame->linesize[0] + j*channel_size + 2];
			data_pointer[i*step_width + j*channel_size + 1] = frame->data[0][i*frame->linesize[0] + j*channel_size + 1];
			data_pointer[i*step_width + j*channel_size + 2] = frame->data[0][i*frame->linesize[0] + j*channel_size + 0];
		}
	}
    cv::Mat resizedMat;
    cv::resize(mat, resizedMat, cv::Size(320, 320 * codec_ctx_->height / codec_ctx_->width));
    imwrite(output_path_ + save_file_name, resizedMat);
    return 0;
}

int main(int argc, const char *argv[]) {
    auto video_path = std::string(argv[1]);
    auto tmp_file = "tmp";
    VideoDecoder decoder;
    int rtn = decoder.Init(video_path, "./", tmp_file);
    cout << "init result " << rtn << endl;
    rtn = decoder.SetDecoderCallback([](string file_name) -> int {
                cout << "decode frame at " << file_name << endl;
                return 0;
            });
    cout << "set decoder result " << rtn << endl;
    //rtn = decoder.TestDecoer(5);
    cout << "test decode result " << rtn << endl;
    rtn = decoder.StartDecode();
    cout << "test start decode result " << rtn << endl;
    return 0;
}
