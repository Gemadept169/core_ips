file(GLOB sources *.cpp)
qt_add_library(entities_grpc_callback STATIC
    ${sources}
)

target_include_directories(entities_grpc_callback 
    PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CORE_INTERNAL_PATH}/sot/include
    ${CORE_3RDPARTY_PATH}/opencv/include/opencv4)
target_link_libraries(entities_grpc_callback
    PUBLIC
    Qt6::Core api utilities sot::sot_base)