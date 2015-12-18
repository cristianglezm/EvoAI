#include <EvoAI/NeuronLayer.hpp>

namespace EvoAI{
    NeuronLayer::NeuronLayer()
    : neurons()
    , biasWeights()
    , bias(0.0){}
    NeuronLayer::NeuronLayer(const std::size_t& numNeurons,const Neuron::Type& t,const double& bias)
    : neurons()
    , biasWeights()
    , bias(bias){}
    std::vector<Neuron>& NeuronLayer::getNeurons(){
        return neurons;
    }
    NeuronLayer& NeuronLayer::setNeurons(std::vector<Neuron>&& ns){
        neurons = std::move(ns);
        biasWeights = std::vector<double>(neurons.size());
        return *this;
    }
    std::size_t NeuronLayer::size(){
        return neurons.size();
    }
    NeuronLayer& NeuronLayer::addNeuron(const Neuron& n){
        neurons.push_back(n);
        return *this;
    }
    NeuronLayer& NeuronLayer::setType(Neuron::Type& t){
        type = t;
        return *this;
    }
    NeuronLayer& NeuronLayer::setBias(const double& bias){
        this->bias = bias;
        return *this;
    }
    bool NeuronLayer::removeNeuron(Neuron* n){
        auto removed = std::remove_if(std::begin(neurons),std::end(neurons),
                                      [&n](Neuron& rn){
                                            return ((*n) == rn);
                                      });
        neurons.erase(removed,std::end(neurons));
        return (removed == std::end(neurons));
    }
    NeuronLayer& NeuronLayer::addConnection(const Connection& c){
        connections.push_back(c);
        return *this;
    }
    bool NeuronLayer::removeConnection(Connection* c){
        auto removed = std::remove_if(std::begin(connections),std::end(connections),
                                      [&c](Connection& rc){
                                            return ((*c) == rc);
                                      });
        connections.erase(removed,std::end(connections));
        return (removed == std::end(connections));
    }
    void NeuronLayer::reset(){
        for(auto& n: neurons){
            n.reset();
        }
    }
    Neuron& NeuronLayer::operator[](const std::size_t& index){
        return neurons[index];
    }
}
