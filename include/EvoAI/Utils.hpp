#ifndef UTILS_HPP

#include <memory>
#include <utility>
#include <EvoAI/NeuralNetwork.hpp>

namespace EvoAI{
    /**
     * @brief 
     * @param numInputs
     * @param numHidden
     * @param bias
     * @return 
     */
    std::unique_ptr<NeuralNetwork> CreateFeedForwardNN(const size_t& numInputs, const size_t& numHidden,
                                                         const size_t& numNeuronsPerHidden, const std::size_t& numOutputs,
                                                         const double& bias){
        std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>(numInputs,numHidden, numNeuronsPerHidden, numOutputs, bias);
        // input to hidden layer
        for(auto i=0u;i<numInputs;++i){
            for(auto j=0u;j<numNeuronsPerHidden;++j){
                nn->addConnection(Connection({0,i},{1,j},0.5));
            }
        }
        //hidden layers to next hidden layer
        for(auto i=1u;i<numHidden;++i){
            for(auto j=0u;j<numNeuronsPerHidden;++j){
                for(auto z=0u;z<numNeuronsPerHidden;++z){
                    nn->addConnection(Connection({i,j},{i+1,z},0.5));
                }
            }
        }
        // last hidden layer to output
        auto numLayers = nn->size();
        for(auto i=0u;i<numNeuronsPerHidden;++i){
            for(auto j=0u;j<numOutputs;++j){
                nn->addConnection(Connection({numLayers-2,i},{numLayers-1,j},0.5));
            }
        }
        return std::move(nn);
    }
    /**
     * @brief 
     * @return 
     */
    std::unique_ptr<NeuralNetwork> createXORNeuralNetwork(){
        std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>(2,1,3,1,1.0);
        nn->addConnection(Connection(Link(0,0),Link(1,0),1.0));
        nn->addConnection(Connection(Link(0,0),Link(1,1),1.0));
        nn->addConnection(Connection(Link(0,1),Link(1,1),1.0));
        nn->addConnection(Connection(Link(0,1),Link(1,2),1.0));
        nn->addConnection(Connection(Link(1,0),Link(2,0),1.0));
        nn->addConnection(Connection(Link(1,1),Link(2,0),-2.0));
        nn->addConnection(Connection(Link(1,2),Link(2,0),1.0));
        return std::move(nn);
    }
}
#endif // UTILS_HPP
