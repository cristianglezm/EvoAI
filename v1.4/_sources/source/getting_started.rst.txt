.. default-domain:: cpp
.. highlight:: cpp

Getting Started
================

Introduction
-------------

EvoAI is a C++17 library to evolve and / or train **small** Neural Networks.

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

    git clone https://github.com/SFML/SFML && git checkout 2.6.x
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

We will make a folder named ``ConsoleApp`` and add the following files.

.. code-block:: cmake

    # CMakeLists.txt
    cmake_minimum_required(VERSION 3.27)
    project(ConsoleApp)
    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
    add_excutable(ConsoleApp main.cpp)
    SET(CMAKE_CXX_STANDARD 17)
    SET(CMAKE_CXX_STANDARD_REQUIRED ON)
    SET(CMAKE_CXX_EXTENSIONS OFF)
    #JsonBox
    find_package(JsonBox REQUIRED)
    if(JsonBox_FOUND)
        target_include_directories(ConsoleApp PRIVATE ${JsonBox_INCLUDE_DIR})
        target_link_libraries(ConsoleApp PRIVATE ${JsonBox_LIBRARIES})
    endif(JsonBox_FOUND)
    #EvoAI
    find_package(EvoAI 1.0 REQUIRED)
    if(EvoAI_FOUND)
        target_include_directories(ConsoleApp PRIVATE ${EvoAI_INCLUDE_DIR})
        target_link_libraries(ConsoleApp PRIVATE ${EvoAI_LIBRARIES})
    endif(EvoAI_FOUND)

then we will make the folders cmake/modules and copy FindEvoAI.cmake and findJsonBox.cmake from EvoAI/cmake/modules/ into it.

.. code-block:: cpp

    // main.cpp
    #include <EvoAI.hpp>
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

then we will make the folder build and use cmake to build and compile the project.

.. code-block:: cpp

    cmake -G"MinGW Makefiles" ..
    mingw32-make -j4
    ./ConsoleApp
    // this will print
    // word is "hello world!"
    // The best EvoString values are "hello world!"
