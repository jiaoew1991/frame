#include <node.h>

#include "./video_wrapper.h"

void InitVideo(Handle<Object> exports) {
    VideoWrapper::Init(exports);
}

NODE_MODULE(video, InitVideo);
