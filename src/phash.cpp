/*
 *  Copyright (c) 2013 Aaron Marasco. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <v8.h>
#include <node.h>
#include <nan.h>
#include "pHash.h"
#include <sstream>
#include <fstream>
#include <cstdio>

using namespace node;
using namespace v8;
using namespace std;

struct PhashRequest {
    string file;
    string hash;
    uv_work_t request;
    Persistent<Function> callback;
};

template <typename T>
string NumberToString ( T Number ) {
    ostringstream ss;
    ss << Number;
    return ss.str();
}

template <typename T>
T StringToNumber ( const string &Text ) {
    istringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
}

const char* toCString(const String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

bool fileExists(const char* filename) {
    ifstream file(filename);
    return file.good();
}

const string getHash(const char* file) {
    // prevent segfault on an empty file, see https://github.com/aaronm67/node-phash/issues/8
    if (!fileExists(file)) {
        return "0";
    }

    string ret;
    try {
        ulong64 hash = 0;
        ph_dct_imagehash(file, hash);
        return NumberToString(hash);
    }
    catch (...) {
        // something went wrong; probably a problem with CImg.
        return "0";
    }
}

//void HashWorker(uv_work_t* req) {
    //PhashRequest* request = static_cast<PhashRequest*>(req->data);
    //request->hash = getHash(request->file.c_str());
//}

//void HashAfter(uv_work_t* req, int status) {
    //HandleScope scope;
    //PhashRequest* request = static_cast<PhashRequest*>(req->data);

    //Handle<Value> argv[2];

    //if (request->hash == "0") {
        //argv[0] = v8::Exception::Error(String::New("Error getting image hash"));
    //}
    //else {
        //argv[0] = Undefined();
    //}

    //argv[1] = String::New(request->hash.c_str());
    //request->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    //request->callback.Dispose();

    //delete request;
//}

class PhashWorker : public NanAsyncWorker {
 public:
    PhashWorker(NanCallback *callback, string file_name)
         : NanAsyncWorker(callback), file(file_name) {}
    ~PhashWorker() {}

    void Execute () {
        hash = getHash(this->file.c_str());
    }

    void HandleOKCallback () {
        NanScope();

        if (this->hash == "0") {
            Local<Value> argv[] = {
                NanError("Error getting image hash")
            };
            callback->Call(1, argv);
        } else {
            Local<Value> argv[] = {
                NanNull(),
                NanNew(hash)
            };
            callback->Call(2, argv);
        }
    };
 private:
    string file;
    string hash;
};


NAN_METHOD(ImageHashAsync) {
    NanScope();

    if (args.Length() < 2 || !args[1]->IsFunction()) {
        // no callback defined
        return NanThrowError("Callback is required and must be an Function.");
    }

    String::Utf8Value str(args[0]);
    //Handle<Function> cb = Handle<Function>::Cast(args[1]);
    auto callback_handle = args[1].As<Function>();
    auto callback = new NanCallback(callback_handle);

    NanAsyncQueueWorker(new PhashWorker(callback, string(*str)));
    //PhashRequest* request = new PhashRequest;
    //request->callback = Persistent<Function>::New(cb);
    //request->file = string(*str);
    //request->request.data = request;
    //uv_queue_work(uv_default_loop(), &request->request, HashWorker, HashAfter);

    NanReturnUndefined();
}

NAN_METHOD(ImageHashSync) {
    NanScope();

    String::Utf8Value str(args[0]);
    string result = getHash(*str);

    NanReturnValue(NanNew(result));
    //return scope.Close(String::New(result.c_str()));
}

NAN_METHOD(HammingDistance) {
    NanScope();

    String::Utf8Value arg0(args[0]);
    String::Utf8Value arg1(args[1]);
    string aString = string(toCString(arg0));
    string bString = string(toCString(arg1));

    ulong64 hasha = StringToNumber<ulong64>(aString);
    ulong64 hashb = StringToNumber<ulong64>(bString);

    int distance = ph_hamming_distance(hasha,hashb);

    NanReturnValue(NanNew<Number>(distance));
}

/*
    See https://github.com/aaronm67/node-phash/issues/4
    V8 only supports 32 bit integers, so hashes must be returned as strings.
    This is a legacy version that returns a 32 bit integer of the hash.
*/
//Handle<Value> oldHash(const Arguments& args) {
    //String::Utf8Value str(args[0]);
    //const char* file = toCString(str);
    //ulong64 hash = 0;
    //ph_dct_imagehash(file, hash);
    //return Number::New(hash);
//}

void RegisterModule(Handle<Object> exports) {
    exports->Set(NanNew("imageHashSync"), NanNew<FunctionTemplate>(ImageHashSync)->GetFunction());
    exports->Set(NanNew("imageHash"), NanNew<FunctionTemplate>(ImageHashAsync)->GetFunction());
    exports->Set(NanNew("hammingDistance"), NanNew<FunctionTemplate>(HammingDistance)->GetFunction());
    //NODE_SET_METHOD(target, "imageHashSync", ImageHashSync);
    //NODE_SET_METHOD(target, "imageHash", ImageHashAsync);
    //NODE_SET_METHOD(target, "hammingDistance", HammingDistance);

    // methods below are deprecated
    //NODE_SET_METHOD(target, "oldHash", oldHash);
    //NODE_SET_METHOD(target, "imagehash", ImageHashSync);
}

NODE_MODULE(pHashBinding, RegisterModule);
