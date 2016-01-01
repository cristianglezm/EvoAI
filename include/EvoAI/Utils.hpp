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
    std::unique_ptr<NeuralNetwork> createFeedForwardNN(const size_t& numInputs, const size_t& numHidden,
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
    std::unique_ptr<NeuralNetwork> createElmanNeuralNetwork(const std::size_t& numInputs, const std::size_t& numHidden, const std::size_t& numNeuronsPerHiddenLayer,
                                                                const std::size_t& numOutputs, const double& bias){
        std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>();
        nn->addLayer(NeuronLayer(numInputs,Neuron::Type::INPUT,bias));
        for(auto i=0u;i<numHidden;++i){
            nn->addLayer(NeuronLayer(numNeuronsPerHiddenLayer,Neuron::Type::CONTEXT,bias));
            nn->addLayer(NeuronLayer(numNeuronsPerHiddenLayer,Neuron::Type::HIDDEN,bias));
        }
        nn->addLayer(NeuronLayer(numOutputs,Neuron::Type::OUTPUT,bias));
        std::cout << "building input connections" << std::endl;
        for(auto i=0u;i<numInputs;++i){
            for(auto j=0u;j<numNeuronsPerHiddenLayer;++j){
                nn->addConnection(Connection(Link(0,i),Link(2,j),1.0));
            }
        }
        std::cout << "building forwards and context connections" << std::endl;
        auto numContext = numHidden * 2;
        for(auto i=2u;i<(numContext-2);i+=2){
            for(auto j=0u;j<numNeuronsPerHiddenLayer;++j){
                // save values to Context
                nn->addConnection(Connection(Link(i,j),Link(i-1,j),1.0));
                for(auto k=0u;k<numNeuronsPerHiddenLayer;++k){
                    // connect forward
                    nn->addConnection(Connection(Link(i,j),Link(i+1,k),0.5));
                    // pass Values from Context backwards
                    nn->addConnection(Connection(Link(i-1,j),Link(i,k),1.0));
                }
            }
        }
        std::cout << "Building Outputs" << std::endl;
        for(auto j=0u;j<numNeuronsPerHiddenLayer;++j){
            // save values to Context
            nn->addConnection(Connection(Link(numContext,j), Link(numContext-1,j),1.0));
            for(auto k=0u;k<numOutputs;++k){
                nn->addConnection(Connection(Link(numContext,j), Link(numContext+1,k),1.0));
            }
            for(auto i=0u;i<numNeuronsPerHiddenLayer;++i){
                // pass Values from Context
                nn->addConnection(Connection(Link(numContext-1,j),Link(numContext,i),1.0));
            }
        }
        std::cout << "Finished Building Elman " << std::endl;
        return std::move(nn);
    }
}
#endif // UTILS_HPP
