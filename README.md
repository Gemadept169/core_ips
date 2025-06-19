<font size= "5"> **Table Of Contents** </font>
- [Todo list](#todo-list)
- [Introduction](#introduction)
- [Folder structure](#folder-structure)
- [Environment for development](#environment-for-development)
  - [At the first time](#at-the-first-time)
    - [System](#system)
    - [Preparation](#preparation)
    - [Qt6](#qt6)
    - [Build and release OpenCV](#build-and-release-opencv)
    - [Build and install gRPC library and dependencies](#build-and-install-grpc-library-and-dependencies)
    - [Discard write permission for all release folders](#discard-write-permission-for-all-release-folders)
  - [At the second times](#at-the-second-times)
    - [Preparation](#preparation-1)
    - [Arrangement](#arrangement)
- [Contributors](#contributors)


# Todo list
- [x] Download Postman and test GrpcServer
- [x] Add SotController and test
- [x] Write a client-side project to evaluate system (Using Qt5, not prefer Qt6)
- [ ] Make grpc communication completely (at both server and client sides)
- [ ] Stop server-streaming sot infor when lost track for 10 frames (read from config file)
- [ ] Rewrite sot library to build with release option.
- [ ] Test bottleneck 
  - [ ] Calculate frame reading rate from rtsp to opencv
  - [ ] Calculate sot processing time
  - [ ] Calculate delay all process
- [ ] Optimize the gst pipeline for capturing video
  - [ ] Decode rtsp pipeline by NVIDIA-GPU and capture streaming video and convert it to cv::Mat (using gstreamer and its plugins)
  - [ ] Auto reconnecting when rtsp streaming failed
- [ ] Write json config parser based on Qt example for game

# Introduction
- A Back-end service for image processing.
# Folder structure
- The structure of folders in the project:
```
<global_path>
|
|--- core_ips
|--- 3rdthirty
    |--- grpc
        |--- bin
        |--- include
        |--- lib
        |--- share
    |--- opencv
        |--- opencv
            |--- bin
            |--- include
            |--- lib
            |--- share
    |--- qt
        |--- 6.7.2, ...
|--- internal
    |--- sot
```
# Environment for development
## At the first time
### System
- Reference to [My Jetson Orin AGX](./docs/jetson_system_information.md)
### Preparation
```
sudo apt update
sudo apt -y install libeigen3-dev libfreetype-dev libharfbuzz-dev cmake libboost-all-dev libx264-dev libsystemd-dev
```

### Qt6
- Crawl Qt framwork corresponding to the machine architecture at <global_path> (following pattern [Folder structure](#folder-structure)).
### Build and release OpenCV 
- To query the number of compute capability of the GPU in the machine architecture. It server to pass to the CUDA_ARCH_BIN argument.
```
nvidia-smi --query-gpu=compute_cap --format=csv # query compute capability of the GPU
```
- Check out *opencv* and *opencv_contrib* repos to *4.10.0* tag
```
cd opencv
rm -rf build/
mkdir build
cd build
cmake \
    -D CMAKE_INSTALL_PREFIX=<global_path>/3rdparty/opencv \
    -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_CXX_FLAGS="-D__STDC_CONSTANT_MACROS" \
    -D BUILD_TESTS=OFF \
    -D BUILD_PERF_TESTS=OFF \
    -D BUILD_EXAMPLES=OFF \
    -D BUILD_opencv_apps=OFF \
    -D BUILD_JAVA=OFF \
    -D BUILD_FAT_JAVA_LIB=OFF \
    -D BUILD_opencv_python2=OFF \
    -D BUILD_opencv_python3=OFF \
    -D BUILD_opencv_cudacodec=ON \
    -D ENABLE_FAST_MATH=ON \
    -D WITH_GTK=OFF 	\
    -D WITH_WIN32UI=OFF 	\
    -D WITH_TBB=ON \
    -D WITH_CUDA=ON \
    -D WITH_CUBLAS=ON \
    -D WITH_NVCUVID=ON \
    -D WITH_NVCUVENC=ON \
    -D WITH_CUDNN=ON \
    -D WITH_FFMPEG=ON \
    -D WITH_GSTREAMER=ON \
    -D HIGHGUI_ENABLE_PLUGINS=OFF	\
    -D CUDA_ARCH_BIN="61 70 75 80 86 87 89" \
    -D CUDA_FAST_MATH=ON \
    -D CUDA_HOST_COMPILER=/usr/bin/g++ \
    -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules \
    -D OPENCV_DNN_CUDA=ON \
    -D OPENCV_ENABLE_NONFREE=ON \
    -D OPENCV_GENERATE_PKGCONFIG=ON \
    ..

make install -j$(nproc)
ldconfig
```
### Build and install gRPC library and dependencies
- Refer: https://github.com/grpc/grpc/blob/master/src/cpp/README.md and https://github.com/grpc/grpc/blob/master/BUILDING.md
```
git clone -b RELEASE_TAG_HERE https://github.com/grpc/grpc
git clone -b v1.66.1 https://github.com/grpc/grpc
cd grpc/
git submodule update --init
mkdir -p cmake/build
cd cmake/build
cmake  \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=<global_path>/3rdparty/grpc \
    -DgRPC_INSTALL=ON \
    -DBUILD_SHARED_LIBS=ON \
    -DgRPC_BUILD_TESTS=OFF \
    ../..
make -j7
make install
```

### Discard write permission for all release folders
```
sudo chmod -R a-w ./3rdparty/*
sudo chmod -R a-w ./internal/*
```

## At the second times
### Preparation
```
sudo apt update
sudo apt -y install libeigen3-dev libfreetype-dev libharfbuzz-dev cmake libboost-all-dev libx264-dev libsystemd-dev
```

### Arrangement
1. Create a *<global_path>* folder at */home/username/*.
2. Copy *release_env.zip* file (packed from *<global_path>* at [At the first time](#at-the-first-time)) to the *Projects* folder; extract the zip here.
3. In the *<global_path>* folder, clone core_ips source.

# Contributors
|  Fullname   | Startdate  |
| :---------: | :--------: |
| Vũ Đức Thái | 03/06/2025 |

