# EvoAI #

A library for Evolving Neural Networks(NEAT and HyperNEAT).

## Building ##

Requires CMake 3.7 and one of the following compilers:

* GCC 5.0 or newer;
* Clang 3.6 or newer (Xcode 5.0 or newer on OS X);
* Visual Studio 2010 or newer;

EvoAI requires the following dependencies:

* [SFML](http://sfml-dev.org) 2.2 or newer (only for Building the tools)

* [JsonBox](https://github.com/anhero/JsonBox) 0.6.2 or newer

* [googletest](https://github.com/google/googletest) 1.7 or newer (only to build Tests)

Use CMake to build the project.

```
mkdir build && cd build
cmake -G"MinGW Makefiles" -DSFML_ROOT=<baseDir> -DJsonBox_ROOT=<baseDir> \ 
-DGMock_ROOT=<baseDir> -DGTEST_ROOT=<baseDir> \ 
-DEvoAI_BUILD_TESTS=TRUE -DEvoAI_BUILD_EXAMPLES=TRUE -DEvoAI_BUILD_DOCS=TRUE -DEvoAI_BUILD_TOOLS=TRUE ..
make -j3 install
```

### Android ###

```
mkdir build
cd build && mkdir armeabi-v7a
cd armeabi-v7a
cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK=<android_ndk> \
        -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a -DCMAKE_ANDROID_STL_TYPE=c++_static \ 
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang \ 
        -DCMAKE_INSTALL_PREFIX=<android_ndk>/sources/third_party/EvoAI ../..
make install
```

You should replace <android_ndk> with the actual folder.

## Tools ##

* GenomeVisualizer: Lets you visualize genomes.
* ImageEvolver: Makes a batch of images and make them reproduce and evolve.
* ImageGenerator: Makes an image from the parameters.
* ImageMixer: mix a number of images together(it takes the resolution from the first image).
* NeuralNetworkVisualizer: Lets you visualize Neural networks.
* SoundGenerator: Makes a sound from the parameters.

## Examples ##

* XOR: Trains or evolves a Neural Network to Solve XOR
* IrisClassifier: Trains or evolves a neural network to classify iris dataset

## Tests ##

* Make tests to check everything is working.

## Contributions ##

Contributions are welcome, go to TODO.md and check out things to add or improve.

## Documentation ##

To make the documentation you will need doxygen and dot.
