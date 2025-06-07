add_subdirectory(grpc_callback)
file(GLOB_RECURSE entities_sources *.cpp)
qt_add_library(entities STATIC
    ${entities_sources}
)
target_include_directories(entities 
    PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CORE_INTERNAL_PATH}/sot/include
    ${CORE_3RDPARTY_PATH}/opencv/include/opencv4)
target_link_libraries(entities 
    PUBLIC 
    ${OpenCV_LIBRARIES} Qt6::Core api utilities sot::sot_base sot::sot_factory sot::sot_siamrpn entities_grpc_callback)

