{
    "includes": [ "deps/common.gyp" ],
    "targets": [
        {
            "target_name": "video",
            "sources": [
                "video.cpp",
                "video_decoder.cpp",
                "video_wrapper.cpp"
            ],
            "conditions": [
                [ "OS=='mac'", {
                    "xcode_settings": {
                        "OTHER_CPLUSPLUSFLAGS" : ["-std=c++11","-stdlib=libc++"],
                        "OTHER_LDFLAGS": ["-stdlib=libc++"],
                        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                        "MACOSX_DEPLOYMENT_TARGET": "10.7"
                    },
                    "libraries": [
                        "/usr/local/lib/libopencv_core.dylib",
                        "/usr/local/lib/libopencv_highgui.dylib",
                        "/usr/local/lib/libavutil.dylib",
                        "/usr/local/lib/libavformat.dylib",
                        "/usr/local/lib/libswscale.dylib",
                        "/usr/local/lib/libswresample.dylib"
                    ]
                }]
            ],
            "cflags": [
                "-std=c++11"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "/usr/local/include"
            ]
        },
        {
            "target_name": "pHashBinding",
            "defines": [
                "HAVE_IMAGE_HASH",
                "cimg_verbosity=0",
                "cimg_use_png",
                "cimg_use_jpeg",
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "deps/pHash",
                "deps/libpng",
                "deps/libjpeg",
            ],
            "sources": [ "phash.cpp" ],
            "dependencies": [
                "deps/zlib/zlib.gyp:zlib",
                "deps/libpng/libpng.gyp:libpng",
                "deps/libjpeg/libjpeg.gyp:libjpeg",
                "deps/pHash/pHash.gyp:phash",
            ],
            "cflags": [
                "-std=c++11"
            ],
            "conditions": [
                [ "OS=='win'",
                    {
                        "include_dirs": [
                            "deps/pHash/win32/",
                        ],
                    },
                ],
            ],
        }

    ]
}
