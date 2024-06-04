#!/bin/bash

PLATFORM=$1
BUILD_TYPE=$2

case "$PLATFORM" in
    Linux*)
        git clone https://github.com/cristianglezm/JsonBox
        cd JsonBox && mkdir build && cd build
        export JsonBox_ROOT=$(pwd)/install
        cmake -DCMAKE_INSTALL_PREFIX=$JsonBox_ROOT -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_CXX_FLAGS=-fPIC ..
        cmake --build . -j 4 -t install
        cd ../..

        git clone https://github.com/google/googletest
        cd googletest && mkdir build && cd build
        export GTest_ROOT=$(pwd)/install
        cmake -DCMAKE_INSTALL_PREFIX=$GTest_ROOT -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_CXX_FLAGS=-fPIC ..
        cmake --build . -j 4 -t install
        cd ../..
    ;;
    Windows*)
        git clone https://github.com/cristianglezm/JsonBox
        cd JsonBox && mkdir build && cd build
        export JsonBox_ROOT=$(pwd)/install
        cmake -DCMAKE_INSTALL_PREFIX=$JsonBox_ROOT -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
        cmake --build . -j 4 -t install
        cd ../..

        git clone https://github.com/google/googletest
        cd googletest && mkdir build && cd build
        export GTest_ROOT=$(pwd)/install
        cmake -DCMAKE_INSTALL_PREFIX=$GTest_ROOT -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
        cmake --build . -j 4 -t install
        cd ../..
    ;;
    mac*)
        git clone https://github.com/cristianglezm/JsonBox
        cd JsonBox && mkdir build && cd build
        export JsonBox_ROOT=$(pwd)/install
        cmake -DCMAKE_INSTALL_PREFIX=$JsonBox_ROOT -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
        cmake --build . -j 4 -t install
        cd ../..

        git clone https://github.com/google/googletest
        cd googletest && mkdir build && cd build
        export GTest_ROOT=$(pwd)/install
        cmake -DCMAKE_INSTALL_PREFIX=$GTest_ROOT -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
        cmake --build . -j 4 -t install
        cd ../..
    ;;
    Android*)
        export ANDROID_NDK=$ANDROID_SDK_ROOT/ndk/26.2.11394342
        export ANDROID_SOURCES=$ANDROID_NDK/sources/third_party
        export ARCH_ABIS="armeabi-v7a arm64-v8a x86 x86_64"
        export JsonBox_ROOT=$ANDROID_SOURCES/JsonBox
        git clone https://github.com/cristianglezm/JsonBox
        cd JsonBox && mkdir build && cd build
	for arch in $ARCH_ABIS;do
            cmake --fresh -DCMAKE_INSTALL_PREFIX=$JsonBox_ROOT \
                            -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
                            -DCMAKE_SYSTEM_NAME=Android \
                            -DCMAKE_ANDROID_NDK=$ANDROID_NDK \
                            -DCMAKE_ANDROID_ARCH_ABI="$arch" \
                            -DCMAKE_ANDROID_STL_TYPE=c++_shared \
                            -DCMAKE_ANDROID_API=33 \
                            -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang \
                            -DCMAKE_CXX_FLAGS=-fPIC ..
            cmake --build . -j 4 -t install
        done
	cd ../..

    ;;
    Emscripten*)
        git clone https://github.com/emscripten-core/emsdk.git
        cd emsdk && ./emsdk install latest && ./emsdk activate latest
        source emsdk_env.sh
        cd ..

        git clone https://github.com/cristianglezm/JsonBox
        cd JsonBox && mkdir build && cd build
        export JsonBox_ROOT=$(pwd)/install
        emcmake cmake -DCMAKE_INSTALL_PREFIX=$JsonBox_ROOT -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
        cmake --build . -j 4 -t install
        cd ../..

        git clone https://github.com/google/googletest
        cd googletest && mkdir build && cd build
        export GTest_ROOT=$(pwd)/install
        emcmake cmake -DCMAKE_INSTALL_PREFIX=$GTest_ROOT -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
        cmake --build . -j 4 -t install
        cd ../..
    ;;
    *)
      echo "usage:"
      echo "$0 <platform> <build_type>"
      echo "where platforms:"
      echo "    Linux"
      echo "    Windows"
      echo "    Mac"
      echo "    Android"
      echo "    Emscripten"
      echo "and build_type:"
      echo "    Release"
      echo "    Debug"
    ;;
esac
