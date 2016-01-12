#include <iostream>
#include <gtest/gtest.h>
#include <EvoAI.hpp>
#include "NeuronTest/NeuronTest.hpp"
#include "NeuronLayerTest/NeuronLayerTest.hpp"
#include "NeuralNetworkTest/NeuralNetworkTest.hpp"

int main(int argc, char** argv){
    std::cout << "Initializing Tests...." << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
