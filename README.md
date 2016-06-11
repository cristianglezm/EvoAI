# EvoAI #

A library for Evolving Neural Networks.

## Building ##

Requires CMake and one of the following compilers:

* GCC 4.9 or newer;
* Clang 3.6 or newer (Xcode 5.0 or newer on OS X);
* Visual Studio 2010 or newer;

EvoAI requires the following dependencies:

* [SFML](http://sfml-dev.org) 2.2 or newer (only for Building the tools)

* [JsonBox](https://github.com/anhero/JsonBox) 0.6.2 or newer

* [googletest](https://github.com/google/googletest) 1.7 or newer (only to build Tests)

Use CMake to build the project.

```
mkdir build && cd build
cmake -G"MinGW Makefiles" -DSFML_ROOT=<baseDir> -DJSONBOX_ROOT=<baseDir> \ 
-DGMOCK_ROOT=<baseDir> -DGTEST_ROOT=<baseDir> \ 
-DEvoAI_BUILD_TESTS=TRUE -DEvoAI_BUILD_EXAMPLES=TRUE -DEvoAI_BUILD_DOCS=TRUE -DEvoAI_BUILD_TOOLS=TRUE ..
make -j3
make install

```

## Contributions ##


## Documentation ##

To make the documentation you will need doxygen and dot.