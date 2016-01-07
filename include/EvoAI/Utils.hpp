#ifndef UTILS_HPP
#define UTILS_HPP

#include <memory>
#include <utility>
#include <random>
#include <EvoAI/NeuralNetwork.hpp>

namespace EvoAI{
    /**
     * @brief 
     * @param min
     * @param max
     * @return 
     */
    double random(const double& min, const double& max){
        using Distribution = std::normal_distribution<double>;
        static std::random_device rd;
        static std::mt19937 rEngine(rd());
        static Distribution dice(min, max);
        return dice(rEngine);
    }
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
                nn->addConnection(Connection({0,i},{1,j}, random(-1.0,1.0)));
            }
        }
        //hidden layers to next hidden layer
        for(auto i=1u;i<numHidden;++i){
            for(auto j=0u;j<numNeuronsPerHidden;++j){
                for(auto z=0u;z<numNeuronsPerHidden;++z){
                    nn->addConnection(Connection({i,j},{i+1,z}, random(-1.0,1.0)));
                }
            }
        }
        // last hidden layer to output
        auto numLayers = nn->size();
        for(auto i=0u;i<numNeuronsPerHidden;++i){
            for(auto j=0u;j<numOutputs;++j){
                nn->addConnection(Connection({numLayers-2,i},{numLayers-1,j}, random(-1.0,1.0)));
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
        for(auto i=0u;i<numInputs;++i){
            for(auto j=0u;j<numNeuronsPerHiddenLayer;++j){
                nn->addConnection(Connection(Link(0,i),Link(2,j), random(-1.0,1.0)));
            }
        }
        auto numContext = numHidden * 2;
        for(auto i=2u;i<numContext;i+=2){
            for(auto j=0u;j<numNeuronsPerHiddenLayer;++j){
                // save values to Context
                nn->addConnection(Connection(Link(i,j),Link(i-1,j),1.0));
                for(auto k=0u;k<numNeuronsPerHiddenLayer;++k){
                    // connect forward
                    nn->addConnection(Connection(Link(i,j),Link(i+2,k), random(-1.0,1.0)));
                    // pass Values from Context backwards
                    nn->addConnection(Connection(Link(i-1,j),Link(i,k), random(-1.0,1.0)));
                }
            }
        }
        for(auto j=0u;j<numNeuronsPerHiddenLayer;++j){
            // save values to Context
            nn->addConnection(Connection(Link(numContext,j), Link(numContext-1,j),1.0));
            for(auto k=0u;k<numOutputs;++k){
                nn->addConnection(Connection(Link(numContext,j), Link(numContext+1,k), random(-1.0,1.0)));
            }
            for(auto i=0u;i<numNeuronsPerHiddenLayer;++i){
                // pass Values from Context
                nn->addConnection(Connection(Link(numContext-1,j),Link(numContext,i), random(-1.0,1.0)));
            }
        }
        return std::move(nn);
    }
    std::unique_ptr<EvoAI::NeuralNetwork> createXORNeuralNetwork(){
        std::unique_ptr<EvoAI::NeuralNetwork> nn = std::make_unique<EvoAI::NeuralNetwork>(2,1,2,1,1.0);
        nn->addConnection(EvoAI::Connection(EvoAI::Link(0,0),EvoAI::Link(1,0),0.07));
        nn->addConnection(EvoAI::Connection(EvoAI::Link(0,0),EvoAI::Link(1,1),0.94));
        
        nn->addConnection(EvoAI::Connection(EvoAI::Link(0,1),EvoAI::Link(1,0),0.22));
        nn->addConnection(EvoAI::Connection(EvoAI::Link(0,1),EvoAI::Link(1,1),0.46));
        
        nn->addConnection(EvoAI::Connection(EvoAI::Link(1,0),EvoAI::Link(2,0),-0.22));
        nn->addConnection(EvoAI::Connection(EvoAI::Link(1,1),EvoAI::Link(2,0),0.58));
        (*nn)[1][0].setBiasWeight(-0.46);
        (*nn)[1][1].setBiasWeight(0.10);
        (*nn)[2][0].setBiasWeight(0.78);
        return std::move(nn);
    }
}
#endif // UTILS_HPP
