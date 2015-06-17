#ifndef VIDEO_WRAPPER_H
#define VIDEO_WRAPPER_H

#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

#include <string>

#include "./video_decoder.h"

using v8::Handle;
using v8::Object;
using v8::Persistent;
using v8::FunctionTemplate;

class VideoWrapper : public node::ObjectWrap {

 public:
    static NAN_METHOD(New);
    static NAN_METHOD(OnProgress);
    static NAN_METHOD(Start);

    static void Init(Handle<Object> exports);

 private:
    ~VideoWrapper() {}

 public:
    static Persistent<FunctionTemplate> constructor;
    VideoDecoder video_decoder_;

};

void InitVideo(Handle<Object> exports);

#endif /* end of include guard: VIDEO_WRAPPER_H */
