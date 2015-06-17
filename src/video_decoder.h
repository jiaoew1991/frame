#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include <list>
#include <iostream>
#include <string>
using namespace std;

//ffmpeg
extern "C" {
#include <stdlib.h>
#include <stdio.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

//typedef int (*DecoderCallback)(string);
typedef std::function<int (string)> DecoderCallback;

class VideoDecoder {

 public:
    VideoDecoder() {}
    virtual ~VideoDecoder() {}

    int Init(std::string, std::string, std::string);
    int SetDecoderCallback(DecoderCallback callback);
    int StartDecode();
    int Release();

    int TestDecoer(int count) {
        AVPacket packet;
        int is_frame_finished;
        int index = 0;
        cout << "begin decode" << endl;
        while (av_read_frame(format_ctx_, &packet) >= 0) {
            if (index >= count) break;
            cout << "stream index " << packet.stream_index << ", format is " << frame_->format << " " << index << endl;
            if (packet.stream_index == video_stream_) {
                avcodec_decode_video2(codec_ctx_, frame_, &is_frame_finished, &packet);
                cout << "is finished: " << is_frame_finished << endl;
                if (is_frame_finished) {
                    int num = frame_->coded_picture_number;
                    sws_scale(sws_ctx_, frame_->data, frame_->linesize, 0, codec_ctx_->height, frame_RGB_->data, frame_RGB_->linesize);
                    if (!this->SaveFrameToJpeg(frame_RGB_, "tmp_" + std::to_string(index) + ".jpg")) {
                        cout << "save to jpeg success" << endl;
                        callback_("tmp_" + std::to_string(index) + ".jpg");
                    }
                    index++;
                }
            }
            av_free_packet(&packet);
        }
        return 0;
    }

 private:
    int SaveFrameToJpeg(AVFrame* frame, std::string save_file_name);

 private:
    std::string video_path_;
    std::string output_path_;
    std::string tmp_file_;
    DecoderCallback callback_;

    AVFormatContext* format_ctx_;
    AVCodecContext* codec_ctx_;
    AVFrame* frame_;
    AVFrame* frame_RGB_;
    SwsContext* sws_ctx_;

    int video_stream_;

};


#endif /* end of include guard: VIDEO_DECODER_H */
