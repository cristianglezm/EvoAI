#include <EvoAI/NeuralNetwork.hpp>

namespace EvoAI{
    NeuralNetwork::NeuralNetwork()
    : layers(){}
    NeuralNetwork::NeuralNetwork(const std::size_t& numInputs,const std::size_t& numHiddenLayers,
                                const std::size_t& numNeuronsPerHiddenLayer,
                                const std::size_t& numOutputs,const double& bias){
        layers.push_back(NeuronLayer(numInputs,Neuron::Type::INPUT,bias));
        for(auto i=0u;i<numHiddenLayers;++i){
            layers.push_back(NeuronLayer(numNeuronsPerHiddenLayer,Neuron::Type::HIDDEN,bias));
        }
        layers.push_back(NeuronLayer(numOutputs,Neuron::Type::OUTPUT,bias));
    }
    NeuralNetwork& NeuralNetwork::addLayer(const NeuronLayer& l){
        layers.push_back(l);
        return *this;
    }
    bool NeuralNetwork::removeLayer(const NeuronLayer& l){
        auto removed = std::remove_if(std::begin(layers),std::end(layers),
                                      [&l](const NeuronLayer& rl){
                                        return (l == rl);
                                      });
        layers.erase(removed,std::end(layers));
        return (removed == std::end(layers));
    }
    NeuralNetwork& NeuralNetwork::setLayers(std::vector<NeuronLayer>&& lys){
        layers = std::move(lys);
        return *this;
    }
    bool NeuralNetwork::setInputs(std::vector<double>&& ins){
        auto numInputs = layers[0].size();
        if(ins.size() != numInputs){
            return false;
        }
        std::vector<Neuron> neurons(numInputs);
        for(auto i=0u;i<numInputs;++i){
            neurons[i].setValue(ins[i]);
        }
        layers[0].setNeurons(std::move(neurons));
        return true;
    }
    NeuralNetwork& NeuralNetwork::addConnection(const Connection& c){
        layers[c.getSrc().layer].addConnection(c);
        return *this;
    }
    bool NeuralNetwork::removeConnection(Connection& c){
        return layers[c.getSrc().layer].removeConnection(c);
    }
}
