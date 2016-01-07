#include <EvoAI/NeuralNetwork.hpp>

namespace EvoAI{
    NeuralNetwork::NeuralNetwork()
    : layers()
    , connections()
    , connectionsCached(false)
    , mse(0.0){}
    NeuralNetwork::NeuralNetwork(const std::size_t& numInputs, const std::size_t& numHiddenLayers,
                                const std::size_t& numNeuronsPerHiddenLayer,
                                const std::size_t& numOutputs, const double& bias)
    : layers()
    , connections()
    , connectionsCached(false)
    , mse(0.0){
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
            auto output = 0.0;
            switch(nrnSrc.getType()){
                case Neuron::Type::INPUT:
                        nrnDest.addSum(nrnSrc.getSum() * w);
                    break;
                case Neuron::Type::CONTEXT:
                case Neuron::Type::HIDDEN:
                        if(nrnSrc.getType() != Neuron::Type::CONTEXT){
                            nrnSrc.addSum(self[src.layer].getBias() * nrnSrc.getBiasWeight());
                        }
                        output = activate(self[src.layer].getActivationType(),nrnSrc);
                        nrnSrc.setOutput(output);
                        nrnDest.addSum(output * w);
                        if(nrnDest.getType() == Neuron::Type::CONTEXT){
                            if(c->getCycles() > self[dest.layer].getCyclesLimit()){
                                nrnDest.resetContext(); /// is this necesary? review
                                c->setCycles(0);
                            }
                            nrnDest.setSum(nrnSrc.getSum());
                            c->setCycles(c->getCycles()+1);
                        }
                    break;
                case Neuron::Type::OUTPUT:{
                        // if output is connected somewhere is to a context if not is wrong.
                        double oldSum = nrnSrc.getSum();
                        nrnSrc.addSum(self[src.layer].getBias() * nrnSrc.getBiasWeight());
                        if(c->getCycles() > self[dest.layer].getCyclesLimit()){
                            nrnDest.resetContext(); /// is this necesary? review
                            c->setCycles(0);
                        }
                        nrnDest.setSum(nrnSrc.getSum());
                        nrnSrc.setSum(oldSum);
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
            n.addSum(outputLayer.getBias() * n.getBiasWeight());
            output = activate(outputLayer.getActivationType(),n);
            n.setOutput(output);
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
            layers[0][i].setSum(ins[i]);
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
                    return Activations::tanh(n.getSum());
                break;
            case NeuronLayer::SINUSOID:
                    return Activations::sinusoid(n.getSum());
                break;
            case NeuronLayer::SIGMOID:
                    return Activations::sigmoid(n.getSum());
                break;
            case NeuronLayer::RELU:
                    return Activations::relu(n.getSum());
                break;
            case NeuronLayer::NOISY_RELU:
                    return Activations::noisyRelu(n.getSum());
                break;
            case NeuronLayer::LEAKY_RELU:
                    return Activations::leakyRelu(n.getSum());
                break;
            case NeuronLayer::EXPONENTIAL:
                    return Activations::exponential(n.getSum());
                break;
            case NeuronLayer::STEPPED_SIGMOID:
            default:
                return Activations::sigmoid(n.getSum(),n.getBiasWeight());
                break;
        }
    }
}
