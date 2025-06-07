# Modern Usage to generate header/source cpp files automatically
# Refer: https://github.com/protocolbuffers/protobuf/blob/main/docs/cmake_protobuf_generate.md
file(GLOB sources "core_ips/*.proto")
add_library(api OBJECT ${sources})
target_link_libraries(api PUBLIC protobuf::libprotobuf gRPC::grpc++)
set(PROTOS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${PROTOS_BINARY_DIR})
target_include_directories(api PUBLIC "$<BUILD_INTERFACE:${PROTOS_BINARY_DIR}>")

protobuf_generate(
    APPEND_PATH TRUE
    TARGET api
    PROTOC_OUT_DIR "${PROTOS_BINARY_DIR}")

protobuf_generate(
    APPEND_PATH TRUE
    TARGET api
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PLUGIN "protoc-gen-grpc=\$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
    PROTOC_OUT_DIR "${PROTOS_BINARY_DIR}")
