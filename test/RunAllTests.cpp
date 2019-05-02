#include <iostream>
#include <gtest/gtest.h>
#include <EvoAI.hpp>
#include "NeuronTest/NeuronTest.hpp"
#include "NeuronLayerTest/NeuronLayerTest.hpp"
#include "NeuralNetworkTest/NeuralNetworkTest.hpp"
#include "ConnectionTest/ConnectionTest.hpp"
#include "GenomeTest/GenomeTest.hpp"
#include "NodeGeneTest/NodeGeneTest.hpp"
#include "ConnectionGeneTest/ConnectionGeneTest.hpp"
#include "PopulationTest/PopulationTest.hpp"
#include "SpeciesTest/SpeciesTest.hpp"
#include "HyperNeatTest/HyperNeatTest.hpp"
#include "UtilsTest/UtilsTest.hpp"
#include "ActivationsTest/ActivationsTest.hpp"

int main(int argc, char** argv){
    std::cout << "Initializing Tests...." << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
