add_subdirectory(grpc_callback)
add_subdirectory(video_capture)
add_subdirectory(inner)
add_subdirectory(system_info)
qt_add_library(entities STATIC)
target_link_libraries(entities
    PUBLIC
    entities_inner 
    entities_grpc_callback 
    entities_video_capture 
    entities_system_info)

