#----------------------------------------------------------------------------------
# THE ROOT CMAKE FILE FOR CORE_IPS APPLICATION
#----------------------------------------------------------------------------------
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "FATAL: In-source builds are not allowed. You should create a separate directory for build files.")
endif()

cmake_minimum_required(VERSION 3.22)

project(core_ips
        VERSION 0.0 
        LANGUAGES CXX
        DESCRIPTION "Image processing service.")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_3RDPARTY_PATH "${CMAKE_SOURCE_DIR}/../3rdparty")
set(CORE_INTERNAL_PATH "${CMAKE_SOURCE_DIR}/../internal")

##############################################################################
### SETUP QT FRAMEWORK
##############################################################################
list(APPEND CMAKE_PREFIX_PATH "${CORE_3RDPARTY_PATH}/qt/6.7.2/gcc_arm64/lib/cmake/Qt6")
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Concurrent)
message ("[${CMAKE_PROJECT_NAME}] [Qt] version   :   ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
message ("[${CMAKE_PROJECT_NAME}] [Qt] include   :   TODO")
message ("[${CMAKE_PROJECT_NAME}] [Qt] libraries :   TODO")
qt_standard_project_setup()

##############################################################################
### SETUP OPENCV LIBRARY
##############################################################################
list(APPEND CMAKE_PREFIX_PATH "${CORE_3RDPARTY_PATH}/opencv/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)
message ("[${CMAKE_PROJECT_NAME}] [OpenCV] version   :   ${OpenCV_VERSION}")
message ("[${CMAKE_PROJECT_NAME}] [OpenCV] include   :   ${OpenCV_INCLUDE_DIRS}")
message ("[${CMAKE_PROJECT_NAME}] [OpenCV] libraries :   ${OpenCV_LIBRARIES}")

##############################################################################
### SETUP GRPC AND ITS DEPENDENCEIS (INCLUDE PROTOBUF)
##############################################################################
list(APPEND CMAKE_PREFIX_PATH "${CORE_3RDPARTY_PATH}/grpc/lib/cmake/absl")
list(APPEND CMAKE_PREFIX_PATH "${CORE_3RDPARTY_PATH}/grpc/lib/cmake/c-ares")
list(APPEND CMAKE_PREFIX_PATH "${CORE_3RDPARTY_PATH}/grpc/lib/cmake/re2")
list(APPEND CMAKE_PREFIX_PATH "${CORE_3RDPARTY_PATH}/grpc/lib/cmake/utf8_range")
list(APPEND CMAKE_PREFIX_PATH "${CORE_3RDPARTY_PATH}/grpc/lib/cmake/protobuf")
list(APPEND CMAKE_PREFIX_PATH "${CORE_3RDPARTY_PATH}/grpc/lib/cmake/grpc")
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

message("[${CMAKE_PROJECT_NAME}] [Protobuf] version: ${Protobuf_VERSION}")
message("[${CMAKE_PROJECT_NAME}] [Protobuf] include: ${Protobuf_INCLUDE_DIRS}")
message("[${CMAKE_PROJECT_NAME}] [Protobuf] libraries: ${PROTOBUF_LIBRARIES}")

message("[${CMAKE_PROJECT_NAME}] [gRPC] version: ${gRPC_VERSION}")
message("[${CMAKE_PROJECT_NAME}] [gRPC] include: ${gRPC_INCLUDE_DIRS}")
message("[${CMAKE_PROJECT_NAME}] [gRPC] libraries: ${gRPC_LIBRARIES}")

##############################################################################
### SETUP SOT LIBRARY
##############################################################################
list(APPEND CMAKE_PREFIX_PATH "${CORE_INTERNAL_PATH}/sot/lib/cmake")
find_package(Sot CONFIG REQUIRED)
message("[${CMAKE_PROJECT_NAME}] [sot] version: ${Sot_VERSION}")
message("[${CMAKE_PROJECT_NAME}] [sot] include: ${Sot_INCLUDE_DIRS}")
message("[${CMAKE_PROJECT_NAME}] [sot] libraries: ${Sot_LIBRARIES}")

##############################################################################
### FOR THE DEBUG PURPOSE
##############################################################################
# get_property(imported_targets_after DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
# message("[${CMAKE_PROJECT_NAME}] Imported targets: ${imported_targets_after}")

add_subdirectory(controllers)
add_subdirectory(entities)
add_subdirectory(main)
add_subdirectory(api)
add_subdirectory(utilities)