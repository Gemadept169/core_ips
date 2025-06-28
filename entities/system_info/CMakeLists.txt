file(GLOB sources *.cpp)
qt_add_library(entities_system_info STATIC
    ${sources}
)
target_include_directories(entities_system_info 
    PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CORE_3RDPARTY_PATH}/opencv/include/opencv4)
target_link_libraries(entities_system_info
    PUBLIC
    Qt6::Core opencv_core opencv_videoio)