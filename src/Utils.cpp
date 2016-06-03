#include <EvoAI/Utils.hpp>

namespace EvoAI{
    double random(const double& min, const double& max){
        using Distribution = std::uniform_real_distribution<double>;
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        //std::random_device{}();
        static std::default_random_engine rEngine(seed);
        static Distribution dice(min, max);
        return dice(rEngine);
    }
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
        return nn;
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
        return nn;
    }
    std::unique_ptr<NeuralNetwork> createFullyConnectedCPPN(const std::size_t& numInputs, const std::size_t& numHidden,
                                                            const std::size_t& numNeuronsPerHiddenLayer, const std::size_t& numOutputs, const double& bias){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        static std::default_random_engine g(seed);
        static std::uniform_int_distribution<int> dice(0,Neuron::ActivationType::LAST_CPPN_ACTIVATION_TYPE-1);
        std::unique_ptr<NeuralNetwork> nn = createFeedForwardNN(numInputs,numHidden,numNeuronsPerHiddenLayer,numOutputs,bias);
        for(auto i=0u;i<nn->size();++i){
            for(auto j=0u;j<(*nn)[i].size();++j){
                (*nn)[i][j].setActivationType(static_cast<Neuron::ActivationType>(dice(g)));
            }
        }
        return nn;
    }
    std::unique_ptr<NeuralNetwork> createCPPN(const std::size_t& numInputs, const std::size_t& numHidden, const std::size_t& numNeuronsPerHiddenLayer, const std::size_t& numOutputs, const double& bias){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        static std::default_random_engine g(seed);
        static std::uniform_int_distribution<int> dice(0,Neuron::ActivationType::LAST_CPPN_ACTIVATION_TYPE-1);
        std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>(numInputs,numHidden,numNeuronsPerHiddenLayer,numOutputs,bias);
        static std::uniform_int_distribution<int> hiddenLayerDice(1,numHidden);
        static std::uniform_int_distribution<int> outputLayerDice(numHidden,numHidden+1);
        static std::uniform_int_distribution<int> inputNeuronDice(0,numInputs-1);
        static std::uniform_int_distribution<int> hiddenNeuronDice(0,numNeuronsPerHiddenLayer-1);
        static std::uniform_int_distribution<int> outputNeuronDice(0,numOutputs-1);
        static std::bernoulli_distribution bernoulli(0.5);
        for(auto i=0u;i<nn->size();++i){
            nn->addConnection(Connection(Link(0,inputNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(0,inputNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(0,inputNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(0,inputNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            
            nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            
            nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(numHidden+1,outputNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(numHidden+1,outputNeuronDice(g)),random(-1.0,1.0)));
            if(bernoulli(g)){
                nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(outputLayerDice(g),outputNeuronDice(g)),random(-1.0,1.0)));
                nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(outputLayerDice(g),outputNeuronDice(g)),random(-1.0,1.0)));
                
                nn->addConnection(Connection(Link(outputLayerDice(g),outputNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
                nn->addConnection(Connection(Link(outputLayerDice(g),outputNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));                
            }
        }
        for(auto i=0u;i<nn->size();++i){
            for(auto j=0u;j<(*nn)[i].size();++j){
                (*nn)[i][j].setActivationType(static_cast<Neuron::ActivationType>(dice(g)));
            }
        }
        return nn;
    }
}