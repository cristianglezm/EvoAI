#include <EvoAI/NeuronLayer.hpp>
#include <EvoAI/Utils.hpp>

namespace EvoAI{
    NeuronLayer::NeuronLayer()
    : neurons()
    , type(Neuron::Type::HIDDEN)
    , activationType(ActivationType::STEPPED_SIGMOID)
    , bias(0.0)
    , cyclesLimit(3){}
    NeuronLayer::NeuronLayer(const std::size_t& numNeurons,const Neuron::Type& t,const double& bias)
    : neurons()
    , type(t)
    , activationType(ActivationType::STEPPED_SIGMOID)
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
    std::size_t NeuronLayer::size(){
        return neurons.size();
    }
    NeuronLayer& NeuronLayer::addNeuron(const Neuron& n){
        neurons.emplace_back(n);
        return *this;
    }
    NeuronLayer& NeuronLayer::setType(Neuron::Type& t){
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
        auto removed = std::remove_if(std::begin(neurons),std::end(neurons),
                                      [&n](Neuron& rn){
                                            return ((*n) == rn);
                                      });
        neurons.erase(removed,std::end(neurons));
        return (removed == std::end(neurons));
    }
    bool NeuronLayer::hasNeuron(Neuron* n){
        auto found = std::find_if(std::begin(neurons),std::end(neurons),
                                        [&n](Neuron& fnrn){
                                            return ((*n) == fnrn);
                                        });
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
    NeuronLayer& NeuronLayer::setActivationType(ActivationType atype){
        activationType = atype;
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
        switch(type){
            case Neuron::Type::CONTEXT: o["neuronType"] = JsonBox::Value("context");   break;
            case Neuron::Type::HIDDEN:  o["neuronType"] = JsonBox::Value("hidden");    break;
            case Neuron::Type::INPUT:   o["neuronType"] = JsonBox::Value("input");     break;
            case Neuron::Type::OUTPUT:  o["neuronType"] = JsonBox::Value("output");    break;
            default:                    o["neuronType"] = JsonBox::Value("undefined"); break;
        }
        switch(activationType){
            case ActivationType::IDENTITY:          o["activationType"] = JsonBox::Value("identity");       break;
            case ActivationType::SIGMOID:           o["activationType"] = JsonBox::Value("sigmoid");        break;
            case ActivationType::STEPPED_SIGMOID:   o["activationType"] = JsonBox::Value("steppedSigmoid"); break;
            case ActivationType::TANH:              o["activationType"] = JsonBox::Value("tanh");           break;
            case ActivationType::SINUSOID:          o["activationType"] = JsonBox::Value("sinusoid");       break;
            case ActivationType::RELU:              o["activationType"] = JsonBox::Value("relu");           break;
            case ActivationType::NOISY_RELU:        o["activationType"] = JsonBox::Value("noisyRelu");      break;
            case ActivationType::LEAKY_RELU:        o["activationType"] = JsonBox::Value("leakyRelu");      break;
            case ActivationType::EXPONENTIAL:       o["activationType"] = JsonBox::Value("exponential");    break;
            default:                                o["activationType"] = JsonBox::Value("undefined");      break;
        }
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
    bool NeuronLayer::operator==(const NeuronLayer& rhs) const{
        return (neurons == rhs.neurons &&
                type == rhs.type &&
                bias == rhs.bias);
    }
}
