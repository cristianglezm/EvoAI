#include <EvoAI/NeuronLayer.hpp>
#include <EvoAI/Utils.hpp>

namespace EvoAI{
    namespace NeuralNetwork{
        NeuronLayer::NeuronLayer()
        : neurons()
        , type(Neuron::Type::HIDDEN)
        , activationType(Neuron::ActivationType::SIGMOID)
        , bias(0.0)
        , cyclesLimit(3){}
        NeuronLayer::NeuronLayer(const std::size_t& numNeurons,const Neuron::Type& t,const double& bias)
        : neurons()
        , type(t)
        , activationType(Neuron::ActivationType::SIGMOID)
        , bias(bias)
        , cyclesLimit(3){
            for(auto i=0u; i<numNeurons;++i){
                neurons.emplace_back(Neuron(t));
                neurons[i].setBiasWeight(random(-1.0,1.0));
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
        NeuronLayer& NeuronLayer::setBias(const double& bias){
            this->bias = bias;
            return *this;
        }
        bool NeuronLayer::removeNeuron(Neuron* n){
            auto removed = std::remove(std::begin(neurons), std::end(neurons), *n);
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
        NeuronLayer& NeuronLayer::setActivationType(Neuron::ActivationType atype){
            activationType = atype;
            for(auto& n: neurons){
                n.setActivationType(atype);
            }
            return *this;
        }
        NeuronLayer& NeuronLayer::setCyclesLimit(const int& cycles){
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
            for(auto& n:neurons){
                a.push_back(n.toJson());
            }
            o["neurons"] = JsonBox::Value(a);
            return JsonBox::Value(o);
        }
        Neuron& NeuronLayer::operator[](const std::size_t& index){
            return neurons[index];
        }
        const Neuron& NeuronLayer::operator[](const std::size_t& index) const{
            return neurons[index];
        }
        bool NeuronLayer::operator==(const NeuronLayer& rhs) const{
            return (std::equal(std::begin(neurons),std::end(neurons),std::begin(rhs.neurons)) &&
                    type == rhs.type &&
                    bias == rhs.bias && 
                    activationType == rhs.activationType);
        }
    }
}