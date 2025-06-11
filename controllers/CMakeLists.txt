file(GLOB sources "*.cpp")
qt_add_library(controllers STATIC
    ${sources}
)
target_include_directories(controllers PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(controllers PUBLIC 
    entities_grpc_callback Qt6::Core gRPC::grpc++)