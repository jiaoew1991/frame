#include <node.h>
#include <nan.h>
#include "./video_wrapper.h"

using v8::Value;
using v8::Local;
using v8::Function;

Persistent<FunctionTemplate> VideoWrapper::constructor;

void VideoWrapper::Init(Handle<Object> exports) {
    NanScope();

    Local<FunctionTemplate> ctor = NanNew<FunctionTemplate>(VideoWrapper::New);
    NanAssignPersistent(constructor, ctor);
    ctor->InstanceTemplate()->SetInternalFieldCount(1);
    ctor->SetClassName(NanNew("Video"));

    NODE_SET_PROTOTYPE_METHOD(ctor, "onProgress", OnProgress);
    NODE_SET_PROTOTYPE_METHOD(ctor, "start", Start);

    exports->Set(NanNew("Video"), ctor->GetFunction());
}

NAN_METHOD(VideoWrapper::New) {
    NanScope();

    auto video = new VideoWrapper();
    video->Wrap(args.This());
    auto video_path = args[0]->ToString();
    auto output_path = args[1]->ToString();
    video->video_decoder_.Init(std::string(*NanAsciiString(video_path)), std::string(*NanAsciiString(output_path)), "tmp");

    NanReturnValue(args.This());
}

NAN_METHOD(VideoWrapper::OnProgress) {
    NanScope();

    VideoWrapper* video = VideoWrapper::Unwrap<VideoWrapper>(args.This());
    auto callback_handle = args[0].As<Function>();
    auto callback = new NanCallback(callback_handle);
    video->video_decoder_.SetDecoderCallback([=](string file_name) -> int {
                //cout << "frame is " << file_name << endl;
                Handle<Value> argv[] = {
                    NanNew<v8::String>(file_name)
                };
                callback->Call(1, argv);
                return 0;
            });

    NanReturnValue(args.This());
}

NAN_METHOD(VideoWrapper::Start) {
    NanScope();

    VideoWrapper* video = VideoWrapper::Unwrap<VideoWrapper>(args.This());
    video->video_decoder_.StartDecode();

    NanReturnValue(args.This());
}

