#include <iostream>
#include <gtest/gtest.h>
#include <EvoAI.hpp>
#include "NeuronTest.hpp"
#include "NeuronLayerTest.hpp"
#include "NeuralNetworkTest.hpp"
#include "ConnectionTest.hpp"
#include "GenomeTest.hpp"
#include "NodeGeneTest.hpp"
#include "ConnectionGeneTest.hpp"
#include "PopulationTest.hpp"
#include "SpeciesTest.hpp"
#include "HyperNeatTest.hpp"
#include "UtilsTest.hpp"
#include "ActivationsTest.hpp"

int main(int argc, char** argv){
    std::cout << "Initializing Tests...." << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
