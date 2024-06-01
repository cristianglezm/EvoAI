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
#include "LossTest.hpp"
#include "SchedulersTest.hpp"
#include "OptimizersTest.hpp"
#include "EvoVectorTest.hpp"

#include <filesystem>

int main(int argc, char** argv){
    std::cout << "Initializing Tests...." << std::endl;
    if(!std::filesystem::directory_entry("testsData").exists()){
        std::filesystem::create_directory("testsData");
    }
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
