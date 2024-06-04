![EvoAI](docs/logo/EvoAILogo.png "EvoAI Logo")

 ![ci](https://github.com/cristianglezm/EvoAI/actions/workflows/ci.yml/badge.svg) [![build-docs](https://github.com/cristianglezm/EvoAI/actions/workflows/docs.yml/badge.svg?branch=master)](https://cristianglezm.github.io/EvoAI/latest/index.html) [![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

EvoAI is a C++17 library to evolve and / or train **small** Neural Networks.

### Features

- A Genome to use NEAT and HyperNEAT algorithms to evolve Neural Networks.
- A Population(with speciation) to evolve your own "Genomes", it only needs to fulfill EvoAI::meta::is_populable_v<myClass>.
- Common Selection Algorithms and optimizers.
- Train neural networks using backpropagation, enabling them to learn from labeled datasets.

## Code Examples

### Evolution

* Evolve an EvoString to the objective word.

```cpp
    std::string word("hello world!");
    constexpr auto numPop = 500u;
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
        std::cout << "The best is ";
        for(auto i=0u;i<best->size();++i){
            std::cout << (*best)[i];
        }
        std::cout << '\0' << std::endl;
    }
```

### Learning

* Train a simple [IrisClassification](examples/IrisClassification/IrisClassification.cpp)

```cpp
    //...
    EvoAI::DataLoader<IrisDataset> trainingSet(IrisDataset(options.dataFilename, true, batchSize));
    EvoAI::DataLoader<IrisDataset> testingSet(IrisDataset(options.dataFilename, false, batchSize));
    //...
    EvoAI::Optimizer optim(0.1, batchSize, EvoAI::SGD(nn->getParameters(), 0.0), EvoAI::Scheduler(EvoAI::MultiStepLR({175}, 0.1)));
    EvoAI::writeMultiPlot("irisAvgLoss.txt", {"epochAvgLoss", "testAvgLoss", "accuracy"},
        nn->train(trainingSet, testingSet, optim, epoch, EvoAI::Loss::MultiClassCrossEntropy{}, testDataset));
    //...
```

## Building ##

Requires CMake 3.7 and one of the following compilers:

* GCC 11.3 or newer; (C++17 support)
* Clang 7.1 or newer (Xcode 5.0 or newer on OS X);
* Visual Studio 2022 or newer;

EvoAI requires the following dependencies:

* [JsonBox](https://github.com/cristianglezm/JsonBox) 0.6.2 or newer

* [SFML](https://github.com/SFML/SFML) 2.6.x (tools and examples)

* [googletest](https://github.com/google/googletest) 1.7 or newer (tests)

Use CMake to build the project, if using vscode disable presets (as of now is not compatible with v7)

```bash
export SFML_ROOT=/full/path/to/sfml
export JsonBox_ROOT=/full/path/to/JsonBox
export GTest_ROOT=/full/path/to/googletest
mkdir build && cd build
cmake -G"MinGW Makefiles" -DSFML_ROOT=$SFML_ROOT -DJsonBox_ROOT=$JsonBox_ROOT -DGTest_ROOT=$GTest_ROOT \ 
-DEvoAI_BUILD_TESTS=TRUE -DEvoAI_BUILD_EXAMPLES=TRUE -DEvoAI_BUILD_DOCS=FALSE -DEvoAI_BUILD_TOOLS=TRUE ..
make -j4 install
```

### Android ###

You should set up ANDROID_NDK with the actual folder.
if you are compiling for termux you will need to compile with ``c++_static`` and 
add a ``-DEvoAI_BUILD_STATIC=TRUE``

```bash
mkdir build
cd build && mkdir armeabi-v7a
cd armeabi-v7a
export ANDROID_NDK=/full/path/to/android_ndk
cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK=$ANDROID_NDK \
        -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a -DCMAKE_ANDROID_STL_TYPE=c++_shared \
        -DCMAKE_ANDROID_API=26 -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang \
        -DJsonBox_ROOT=$ANDROID_NDK/sources/third_party/JsonBox \
        -DCMAKE_INSTALL_PREFIX=$ANDROID_NDK/sources/third_party/EvoAI ../..
make -j4 install
```
### Emscripten ###

You can also build the library for emscripten to use it in the browser.

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
cd ..
git clone https://github.com/cristianglezm/JsonBox.git
cd JsonBox && mkdir build-em && cd build-em
emcmake cmake -G"MinGW Makefiles" ..
make -j4 install
cd ../..
export JsonBox_ROOT=/path/to/jsonbox
export GTest_ROOT=/path/to/googletest
emcmake cmake -G"MinGW Makefiles" -DJSonBox_ROOT=$JsonBox_ROOT -DGTest_ROOT=$GTest_ROOT -DEvoAI_BUILD_TESTS=TRUE ..
make -j4
```

Copy the following html code into build/test.html and run ``emrun test.html`` from the build dir after building it
then check the console log using webdev tools or use node to run it ``node bin/RunAllTests.js``

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <title>WASM EvoAI-RunAllTests Example</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
</head>
<body>
    <div>
      <h1>WASM EvoAI-RunAllTests Example</h1>
    </div>
    <script type='text/javascript'>
        var Module = {};
        fetch('bin/RunAllTests.wasm')
            .then(response => response.arrayBuffer())
            .then(buffer => {
                Module.wasmBinary = buffer;
                var script = document.createElement('script');
                script.src = 'bin/RunAllTests.js';
                script.onload = function() {
                  console.log('Emscripten boilerplate loaded.');
                }
                document.body.appendChild(script);
            });
    </script>
</body>
</html>
```

## Tools ##

* [GenomeVisualizer](tools/GenomeVisualizer): It lets you visualize genomes.
* [ImageEvolver](tools/ImageEvolver): Makes a batch of images and make them reproduce and evolve.
* [ImageGenerator](tools/ImageGenerator): Makes an image from the parameters.
* [ImageMixer](tools/ImageMixer): mix a number of images together(it takes the resolution from the first image).
* [NeuralNetworkVisualizer](tools/NeuralNetworkVisualizer): It lets you visualize Neural networks and produce a dot file.
* [SoundGenerator](tools/SoundGenerator): Makes a sound / midi file from the parameters.

## Examples ##

* [XOR](examples/XOR): Trains or evolves a Neural Network to Solve XOR
* [NumClassifier](examples/NumClassifier): trains a neural network to classify numbers.
* [IrisClassification](examples/IrisClassification): Trains or evolves a neural network to classify iris dataset
* [CellSim](examples/CellSim): Evolve different types of cells and see who takes over.

## Tests ##

* Make tests to check everything is working.

```bash
make RunAllTests && bin/RunAllTests
```

* If you don't want to run all tests use --gtest_filter "test.example", use --gtest_list_tests to check all the available tests.

## Contributions ##

Contributions are welcome, go to TODO.md and check out things to add or improve.

## Documentation ##

[latest version](https://cristianglezm.github.io/EvoAI/latest/index.html)

## License

Copyright 2015-2024 Cristian Gonzalez cristian.glez.m@gmail.com

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
