# EvoAI #

A library for Evolving Neural Networks.

## Building ##

Requires CMake and one of the following compilers:

* GCC 4.9 or newer;
* Clang 3.6 or newer (Xcode 5.0 or newer on OS X);
* Visual Studio 2010 or newer;

EvoAI requires the following dependencies:

* [SFML](http://sfml-dev.org) 2.2 or newer

* [JsonBox](https://github.com/anhero/JsonBox) 0.6.2 or newer

* [googletest](https://github.com/google/googletest) 1.7 or newer

Use CMake to build the project.

```
mkdir build && cd build
cmake -G"MinGW Makefiles" -DSFML_ROOT=<baseDir> -DJSONBOX_ROOT=<baseDir> -DBUILD_TESTS=TRUE -DBUILD_EXAMPLES=TRUE -DBUILD_DOCS=TRUE -DBUILD_TOOLS=TRUE ..
make
make install

```

## Contributions ##


## Documentation ##

To make the documentation you will need doxygen amd dot.