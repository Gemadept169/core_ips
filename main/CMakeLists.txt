file(GLOB main_sources "*.cpp")

qt_add_executable(${CMAKE_PROJECT_NAME}
  ${main_sources}
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
  ${CMAKE_SOURCE_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC 
  controllers Qt6::Core gRPC::grpc++)