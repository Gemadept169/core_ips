file(GLOB sources *.cpp)
qt_add_library(entities_video_capture STATIC
    ${sources}
)
target_include_directories(entities_video_capture 
    PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CORE_3RDPARTY_PATH}/opencv/include/opencv4)
target_link_libraries(entities_video_capture
    PUBLIC
    Qt6::Core opencv_core opencv_videoio)