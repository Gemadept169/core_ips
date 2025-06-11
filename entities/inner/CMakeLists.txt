file(GLOB sources *.cpp)
qt_add_library(entities_inner STATIC
    ${sources}
)
target_include_directories(entities_inner 
    PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CORE_INTERNAL_PATH}/sot/include
    ${CORE_3RDPARTY_PATH}/opencv/include/opencv4)
target_link_libraries(entities_inner 
    PUBLIC 
    ${OpenCV_LIBRARIES} Qt6::Core sot::sot_base sot::sot_factory sot::sot_siamrpn)

