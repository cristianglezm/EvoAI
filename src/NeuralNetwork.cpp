#include <EvoAI/NeuralNetwork.hpp>

namespace EvoAI{
    NeuralNetwork::NeuralNetwork()
    : layers()
    , bias(1.0){}
    NeuralNetwork::NeuralNetwork(const std::size_t& numInputs,const std::size_t& numHiddenLayers,
                                const std::size_t& numNeuronsPerHiddenLayer,
                                const std::size_t& numOutputs,const double bias){
        layers.push_back(NeuronLayer(numInputs,Neuron::INPUT,bias));
        for(int i=0;i<numHiddenLayers;++i){
            layers.push_back(NeuronLayer(numNeuronsPerHiddenLayer,Neuron::HIDDEN,bias));
        }
        layers.push_back(NeuronLayer(numOutputs,Neuron::OUTPUT,bias));
    }
    NeuralNetwork& NeuralNetwork::addLayer(const NeuronLayer& l){
        layers.push_back(l);
        return *this;
    }
    bool NeuralNetwork::removeLayer(const NeuronLayer& l){
        auto removed = std::remove_if(std::begin(layers),std::end(layers),
                                      [&l](auto rl){
                                        return (l == rl);
                                      });
        layers.erase(removed,std::end(layers));
        return (removed == std::end(layers));
    }
    NeuralNetwork& NeuralNetwork::setLayers(std::vector<NeuronLayer>&& lys){
        layers = std:move(lys);
    }
    NeuralNetwork& NeuralNetwork::setInputs(std::vector<double>&& ins){
        if(Ins.size() != layers[0].size())
            return *this;
        std::vector<Neuron> neurons(numInputs);
        for(int i=0;i<numInputs;++i){
            neurons[i].setValue(ins[i]);
        }
        layers[0].setNeurons(std::move(neurons));
        return *this;
    }
    NeuralNetwork& NeuralNetwork::addConnection(const Connection& c){
        layers[c.getSrc().layer].addConnection(c);
    }
    bool NeuralNetwork::removeConnection(const Connection& c){
        layers[c.getSrc().layer].removeConnection(c);
    }
}
