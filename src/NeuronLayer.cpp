#include <EvoAI/NeuronLayer.hpp>

namespace EvoAI{
    NeuronLayer::NeuronLayer()
    : neurons()
    , bias(0.0){}
    NeuronLayer::NeuronLayer(const std::size_t& numNeurons,const Neuron::Type& t,const double& bias)
    : neurons()
    , bias(bias){
        for(auto i=0u; i<numNeurons;++i){
            neurons.push_back(Neuron(t));
        }
    }
    std::vector<Neuron>& NeuronLayer::getNeurons(){
        return neurons;
    }
    NeuronLayer& NeuronLayer::setNeurons(std::vector<Neuron>&& ns){
        neurons = std::move(ns);
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
        neurons[c.getSrc().neuron].addConnection(c);
        return *this;
    }
    bool NeuronLayer::removeConnection(Connection& c){
        return neurons[c.getSrc().neuron].removeConnection(c);
    }
    void NeuronLayer::reset(){
        for(auto& n: neurons){
            n.reset();
        }
    }
    void NeuronLayer::resetContext(){
        for(auto& n:neurons){
            n.resetContext();
        }
    }
    Neuron& NeuronLayer::operator[](const std::size_t& index){
        return neurons[index];
    }
    bool NeuronLayer::operator==(const NeuronLayer& rhs) const{
        return (neurons == rhs.neurons &&
                type == rhs.type &&
                bias == rhs.bias);
    }
}
