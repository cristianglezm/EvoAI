.. default-domain:: cpp
.. highlight:: cpp

Getting Started
================

Introduction
-------------

EvoAI is a C++17 library of building blocks to evolve, train and cluster **small** models, neural networks
that are evolved (NEAT, HyperNEAT) and / or trained (backpropagation), self-organizing maps, and the algorithms around them.

Building on Windows
--------------------

We will first build the dependencies and then build a small console app.

we will need to install msys2 follow `this tutorial`_ after installing msys2 
and installing minGW you will need to add it to the PATH

.. _`this tutorial`: https://www.msys2.org/

JsonBox
^^^^^^^^

we will clone ``JsonBox`` and then compile and install it.

.. code-block:: bash

    git clone https://github.com/cristianglezm/JsonBox
    cd JsonBox && mkdir build && cd build
    cmake -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=./install ..
    mingw32-make -j4 install
    cd ../..

SFML (Optional)
^^^^^^^^^^^^^^^^^

we will clone ``SFML`` and then compile and install it.

.. code-block:: bash

    git clone --branch 2.6.x https://github.com/SFML/SFML
    cd SFML && mkdir build && cd build
    cmake -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=./install ..
    mingw32-make -j4 install
    cd ../..


googletest (Optional)
^^^^^^^^^^^^^^^^^^^^^^

We will clone ``googletest`` and then compile and install it.

.. code-block:: bash

    git clone https://github.com/google/googletest
    cd googletest && mkdir build && cd build
    cmake -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=./install ..
    mingw32-make -j4 install
    cd ../..


EvoAI
^^^^^^

We will clone ``EvoAI`` and then compile and install it, if you don't want to add the optional dependencies
set the ``EvoAI_BUILD_*=FALSE``.

.. code-block:: bash

    git clone https://github.com/cristianglezm/EvoAI
    export SFML_ROOT=SFML/build/install
    export JSONBOX_ROOT=JsonBox/build/install
    export GTest_ROOT=googletest/build/install
    cd EvoAI && mkdir build && cd build
    cmake -G"MinGW Makefiles" -DSFML_ROOT=$SFML_ROOT -DJsonBox_ROOT=$JSONBOX_ROOT -DGTest_ROOT=$GTest_ROOT \ 
    -DEvoAI_BUILD_TESTS=TRUE -DEvoAI_BUILD_EXAMPLES=TRUE -DEvoAI_BUILD_DOCS=TRUE -DEvoAI_BUILD_TOOLS=TRUE \
    -DCMAKE_INSTALL_PREFIX=./install ..
    mingw32-make -j4 install

ConsoleApp
------------

We will make a folder named ``ConsoleApp`` with the following two files. EvoAI installs a CMake package, so it
is found with ``find_package`` and used through its imported target ``EvoAI::EvoAI``: there is nothing to copy
from the EvoAI sources, and the package finds JsonBox again by itself.

.. code-block:: cmake

    # CMakeLists.txt
    cmake_minimum_required(VERSION 3.14)
    project(ConsoleApp LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    find_package(EvoAI 1.0 CONFIG REQUIRED)

    add_executable(ConsoleApp main.cpp)
    target_link_libraries(ConsoleApp PRIVATE EvoAI::EvoAI)

.. code-block:: cpp

    // main.cpp
    #include <EvoAI.hpp>
    #include <iomanip>
    #include <string>
    #include <iostream>

    int main(){
        std::string word("hello world!");
        std::cout << "word is " << std::quoted(word) << std::endl;
        constexpr auto numPop = 500u;
        // create a Population of EvoString, 
        // the first arg is the max population, 
        // the next three are the coefficients and the last is the EvoString constructor argument
        EvoAI::Population<EvoAI::EvoString> pop(numPop,2.0,2.0,1.0, word.size());
        auto correct = 0u;
        auto eval = [&word, &correct](auto& w){
            correct = 0u;
            w.mutate();
            for(auto i=0u;i<word.size();++i){
                if(word[i] == w[i]){
                    correct += 1;
                }
            }
            w.setFitness(correct);
        };
        do{
            pop.eval(eval);
            if(correct != word.size()){
                pop.reproduce(EvoAI::SelectionAlgorithms::Tournament<EvoAI::EvoString>{numPop}, true);
                pop.increaseAgeAndRemoveOldSpecies();
                pop.regrowPopulation(2.0, 2.0, 1.0, word.size());
            }
        }while(correct != word.size());
        auto best = pop.getBestMember();
        if(best){
            std::cout << "The best EvoString values are \"";
            for(auto i=0u;i<best->size();++i){
                std::cout << (*best)[i];
            }
            std::cout << "\"" << '\0' << std::endl;
        }
    }

Then we will configure, build and run it. ``CMAKE_PREFIX_PATH`` has the folders ``EvoAI`` and ``JsonBox`` were
installed in (their ``CMAKE_INSTALL_PREFIX``), separated by ``;``. On Linux and macOS leave out the ``-G``.

.. code-block:: bash

    cmake -S . -B build -G"MinGW Makefiles" \
          "-DCMAKE_PREFIX_PATH=/full/path/to/EvoAI/build/install;/full/path/to/JsonBox/build/install"
    cmake --build build
    ./build/ConsoleApp
    # this will print
    # word is "hello world!"
    # The best EvoString values are "hello world!"

It is the same ``CMakeLists.txt`` for a shared and for a static EvoAI (``-DEvoAI_BUILD_STATIC=TRUE`` when you
built it): ``find_package`` finds the one that is installed. If a prefix holds both, the shared library is used
unless you set ``EvoAI_BUILD_STATIC`` to ``TRUE`` before ``find_package(EvoAI)``.

If JsonBox is somewhere else, ``find_package(EvoAI)`` stops saying it could not find JsonBox: add its folder to
``CMAKE_PREFIX_PATH``, or set ``JsonBox_DIR`` to the folder with its ``JsonBoxConfig.cmake``
(``<prefix>/lib/cmake/JsonBox``). This needs a JsonBox that installs its CMake package; an EvoAI built against an
older JsonBox, or by a toolchain that only looks inside its sysroot (Android, Emscripten), does not find JsonBox for
you: find it and link it yourself.

Using FetchContent
^^^^^^^^^^^^^^^^^^^

To build JsonBox and EvoAI together with your project, without installing them first, use ``FetchContent``.
Here the target is plain ``EvoAI`` (the namespaced ``EvoAI::EvoAI`` is for installed packages), and it is
built static so there is no library to ship next to the executable.

.. code-block:: cmake

    # CMakeLists.txt
    cmake_minimum_required(VERSION 3.14)
    project(ConsoleApp LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    include(FetchContent)
    set(JsonBox_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(JsonBox_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(EvoAI_BUILD_STATIC TRUE CACHE BOOL "" FORCE)
    set(EvoAI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(EvoAI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(EvoAI_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(EvoAI_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(JsonBox GIT_REPOSITORY https://github.com/cristianglezm/JsonBox.git GIT_TAG 0.6.3)
    FetchContent_Declare(EvoAI GIT_REPOSITORY https://github.com/cristianglezm/EvoAI.git GIT_TAG v1.5)
    FetchContent_MakeAvailable(JsonBox EvoAI)

    add_executable(ConsoleApp main.cpp)
    target_link_libraries(ConsoleApp PRIVATE EvoAI)
