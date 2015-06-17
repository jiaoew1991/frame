cmake_minimum_required(VERSION 2.8.12)

PROJECT(main)

find_package(OpenCV REQUIRED)

#add_executable(test_orb src/test.cpp)
#add_executable(test_brief src/test_brief.cpp)

add_compile_options(-std=c++11)

add_executable(test_video_decoder src/video_decoder.cpp)
add_executable(jd_video ./test/jd.cpp)

#target_link_libraries(test_orb ${OpenCV_LIBS})
#target_link_libraries(test_brief ${OpenCV_LIBS})
target_link_libraries(test_video_decoder
    avutil avcodec swscale avformat ${OpenCV_LIBS}
    )
target_link_libraries(jd_video
    avutil avcodec swscale avformat ${OpenCV_LIBS}
    )


