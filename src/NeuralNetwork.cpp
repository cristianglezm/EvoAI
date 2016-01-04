#include <EvoAI/NeuralNetwork.hpp>

namespace EvoAI{
    NeuralNetwork::NeuralNetwork()
    : layers()
    , connections()
    , connectionsCached(false){}
    NeuralNetwork::NeuralNetwork(const std::size_t& numInputs, const std::size_t& numHiddenLayers,
                                const std::size_t& numNeuronsPerHiddenLayer,
                                const std::size_t& numOutputs, const double& bias)
    : layers()
    , connections()
    , connectionsCached(false){
        layers.emplace_back(NeuronLayer(numInputs,Neuron::Type::INPUT,bias));
        for(auto i=0u;i<numHiddenLayers;++i){
            layers.emplace_back(NeuronLayer(numNeuronsPerHiddenLayer,Neuron::Type::HIDDEN,bias));
        }
        layers.emplace_back(NeuronLayer(numOutputs,Neuron::Type::OUTPUT,bias));
    }
    NeuralNetwork& NeuralNetwork::addLayer(const NeuronLayer& l){
        layers.emplace_back(l);
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
    std::vector<double> NeuralNetwork::run(){
        std::vector<double> res;
        auto& self = *this;
        for(auto& c:getConnections()){
            auto w = c->getWeight();
            auto& src = c->getSrc();
            auto& dest = c->getDest();
            auto& nrnSrc = self[src.layer][src.neuron];
            auto& nrnDest = self[dest.layer][dest.neuron];
            auto input = 0.0;
            switch(nrnSrc.getType()){
                case Neuron::Type::INPUT:
                        nrnDest.addValue(nrnSrc.getValue() * w);
                    break;
                case Neuron::Type::CONTEXT:
                case Neuron::Type::HIDDEN:
                        if(nrnSrc.getType() != Neuron::Type::CONTEXT){
                            nrnSrc.addValue(self[src.layer].getBias() * nrnSrc.getBiasWeight());
                        }
                        input = activate(self[src.layer].getActivationType(),nrnSrc);
                        if(nrnDest.getType() != Neuron::Type::CONTEXT){
                            nrnDest.addValue(input * w);
                        }else{
                            if(c->getCycles() > self[dest.layer].getCyclesLimit()){
                                nrnDest.resetContext();
                                c->setCycles(0);
                            }
                            nrnDest.addValue(input * w);
                            c->setCycles(c->getCycles()+1);
                        }
                    break;
                case Neuron::Type::OUTPUT:
                        nrnSrc.addValue(self[src.layer].getBias() * nrnSrc.getBiasWeight());
                        input = activate(self[src.layer].getActivationType(),nrnSrc);
                        if(nrnDest.getType() != Neuron::Type::CONTEXT){
                            nrnDest.addValue(input * w);
                        }else{
                            if(c->getCycles() > self[dest.layer].getCyclesLimit()){
                                nrnDest.resetContext();
                                c->setCycles(0);
                            }
                            nrnDest.addValue(input * w);
                            c->setCycles(c->getCycles()+1);
                        }
                    break;
                default:
                    break;
            }
        }
        auto numLayers = size();
        auto& outputLayer = self[numLayers-1];
        auto output = 0.0;
        for(auto& n:outputLayer.getNeurons()){
            n.addValue(outputLayer.getBias() * n.getBiasWeight());
            output = activate(outputLayer.getActivationType(),n);
            res.emplace_back(output);
        }
        return std::move(res);
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
        for(auto i=0u;i<numInputs;++i){
            layers[0][i].setValue(ins[i]);
        }
        return true;
    }
    NeuralNetwork& NeuralNetwork::addConnection(const Connection& c){
        layers[c.getSrc().layer].addConnection(c);
        connectionsCached = false;
        return *this;
    }
    bool NeuralNetwork::removeConnection(Connection& c){
        connectionsCached = false;
        return layers[c.getSrc().layer].removeConnection(c);
    }
    std::vector<Connection*>& NeuralNetwork::getConnections(){
        if(connectionsCached){
            return connections;
        }
        for(auto& l:layers){
            for(auto& n:l.getNeurons()){
                for(auto& c:n.getConnections()){
                    connections.emplace_back(&c);
                }
            }
        }
        connectionsCached = true;
        return connections;
    }
    Connection* NeuralNetwork::findConnection(Link&& src,Link&& dest){
        auto& self = *this;
        auto& conns = self[src.layer][src.neuron].getConnections();
        auto c = std::find_if(std::begin(conns),std::end(conns),
                            [&src,&dest](const Connection& cn){
                                return (src == cn.getSrc() &&
                                        dest == cn.getDest());
                            });
        if(c != std::end(conns)){
            return &(*c);
        }
        return nullptr;
    }
    bool NeuralNetwork::hasConnection(Link&& src, Link&& dest){
        auto& self = *this;
        auto& conns = self[src.layer][src.neuron].getConnections();
        auto c = std::find_if(std::begin(conns),std::end(conns),
                            [&src,&dest](const Connection& cn){
                                return (src == cn.getSrc() &&
                                        dest == cn.getDest());
                            });
        return (c != std::end(conns));
    }
    void NeuralNetwork::reset(){
        for(auto& l:layers){
            l.reset();
        }
    }
    void NeuralNetwork::resetContext(){
        for(auto& l:layers){
            l.resetContext();
        }
    }
    NeuronLayer& NeuralNetwork::operator[](const std::size_t& index){
        return layers[index];
    }
//private member functions
    const double NeuralNetwork::activate(NeuronLayer::ActivationType at, const Neuron& n) const{
        switch(at){
            case NeuronLayer::TANH:
                    return Activations::tanh(n.getValue());
                break;
            case NeuronLayer::SINUSOID:
                    return Activations::sinusoid(n.getValue());
                break;
            case NeuronLayer::SIGMOID:
                    return Activations::sigmoid(n.getValue());
                break;
            case NeuronLayer::RELU:
                    return Activations::relu(n.getValue());
                break;
            case NeuronLayer::NOISY_RELU:
                    return Activations::noisyRelu(n.getValue());
                break;
            case NeuronLayer::LEAKY_RELU:
                    return Activations::leakyRelu(n.getValue());
                break;
            case NeuronLayer::EXPONENTIAL:
                    return Activations::exponential(n.getValue());
                break;
            case NeuronLayer::STEPPED_SIGMOID:
            default:
                return Activations::sigmoid(n.getValue(),n.getBiasWeight());
                break;
        }
    }
}
