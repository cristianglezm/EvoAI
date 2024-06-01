#include <EvoAI/NeuronLayer.hpp>
#include <EvoAI/Utils.hpp>

namespace EvoAI{
    NeuronLayer::NeuronLayer()
    : neurons()
    , type(Neuron::Type::HIDDEN)
    , activationType(Neuron::ActivationType::SIGMOID)
    , bias(1.0)
    , cyclesLimit(3){}
    NeuronLayer::NeuronLayer(std::size_t numNeurons,const Neuron::Type& t,double Bias)
    : neurons()
    , type(t)
    , activationType(Neuron::ActivationType::SIGMOID)
    , bias(Bias)
    , cyclesLimit(3){
        neurons.reserve(numNeurons);
        for(auto i=0u; i<numNeurons;++i){
            neurons.emplace_back(Neuron(t));
            neurons[i].setBiasWeight(bias);
        }
    }
    NeuronLayer::NeuronLayer(JsonBox::Object o)
    : neurons()
    , type(Neuron::typeToEnum(o["neuronType"].getString()))
    , activationType(Neuron::activationTypeToEnum(o["activationType"].getString()))
    , bias(o["bias"].getDouble())
    , cyclesLimit(o["cyclesLimit"].getInteger()){
        auto& nrns = o["neurons"].getArray();
        neurons.reserve(nrns.size());
        for(auto& n:nrns){
            neurons.emplace_back(n.getObject());
        }
    }
    std::vector<Neuron>& NeuronLayer::getNeurons(){
        return neurons;
    }
    NeuronLayer& NeuronLayer::setNeurons(std::vector<Neuron>&& ns){
        neurons = std::move(ns);
        return *this;
    }
    std::size_t NeuronLayer::size() const{
        return neurons.size();
    }
    NeuronLayer& NeuronLayer::addNeuron(const Neuron& n){
        neurons.emplace_back(n);
        return *this;
    }
    NeuronLayer& NeuronLayer::setType(Neuron::Type t){
        type = t;
        for(auto& n:neurons){
            n.setType(t);
        }
        return *this;
    }
    NeuronLayer& NeuronLayer::setBias(double Bias){
        this->bias = Bias;
        for(auto& n:neurons){
            n.setBiasWeight(bias);
        }
        return *this;
    }
    bool NeuronLayer::removeNeuron(Neuron* n){
        auto removed = std::remove_if(std::begin(neurons), std::end(neurons),
            [n](const Neuron& n2){
                return n == &n2;
            });
        auto isRemoved = removed != std::end(neurons);
        neurons.erase(removed, std::end(neurons));
        return isRemoved;
    }
    bool NeuronLayer::hasNeuron(Neuron* n){
        auto found = std::find(std::begin(neurons),std::end(neurons),*n);
        return (found != std::end(neurons));
    }
    NeuronLayer& NeuronLayer::addConnection(const Connection& c){
        neurons[c.getSrc().neuron].addConnection(c);
        return *this;
    }
    bool NeuronLayer::removeConnection(const Connection& c){
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
    NeuronLayer& NeuronLayer::setActivationType(Neuron::ActivationType atype){
        activationType = atype;
        for(auto& n: neurons){
            n.setActivationType(atype);
        }
        return *this;
    }
    NeuronLayer& NeuronLayer::setCyclesLimit(int cycles){
        cyclesLimit = cycles;
        return *this;
    }
    JsonBox::Value NeuronLayer::toJson() const{
        JsonBox::Object o;
        o["bias"] = JsonBox::Value(bias);
        o["cyclesLimit"] = JsonBox::Value(cyclesLimit);
        o["neuronType"] = JsonBox::Value(Neuron::typeToString(type));
        o["activationType"] = JsonBox::Value(Neuron::activationTypeToString(activationType));
        JsonBox::Array a;
        a.reserve(neurons.size());
        for(auto& n:neurons){
            a.emplace_back(n.toJson());
        }
        o["neurons"] = JsonBox::Value(a);
        return JsonBox::Value(o);
    }
    Neuron& NeuronLayer::operator[](std::size_t index){
        return neurons[index];
    }
    const Neuron& NeuronLayer::operator[](std::size_t index) const{
        return neurons[index];
    }
    bool NeuronLayer::operator==(const NeuronLayer& rhs) const{
        return (std::equal(std::begin(neurons),std::end(neurons),std::begin(rhs.neurons)) &&
                type == rhs.type &&
                bias == rhs.bias && 
                activationType == rhs.activationType);
    }
    std::vector<double> NeuronLayer::backward() const noexcept{
        std::vector<double> grads;
        grads.reserve(size());
        for(const auto& n:neurons){
            grads.emplace_back(n.getGradient());
        }
        return grads;
    }
}
