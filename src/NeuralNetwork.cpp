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
    NeuralNetwork::NeuralNetwork(const std::string& filename)
    : layers()
    , connections()
    , connectionsCached(false)
    , mse(0.0){
        //// TODO
        JsonBox::Value v;
        v.loadFromFile(filename);
        auto layersArray = v["NeuralNetwork"]["layers"].getArray();
        //build layer
        for(auto& la:layersArray){
            NeuronLayer l;
            l.setCyclesLimit(la["cyclesLimit"].getInteger());
            l.setBias(la["bias"].getDouble());
            auto type = la["neuronType"].getString();
            Neuron::Type lyrType = Neuron::Type::HIDDEN;
            if(type == "input"){
                lyrType = Neuron::Type::INPUT;
            }else if(type == "context"){
                lyrType = Neuron::Type::CONTEXT;
            }else if(type == "hidden"){
                lyrType = Neuron::Type::HIDDEN;
            }else if(type == "output"){
                lyrType = Neuron::Type::OUTPUT;
            }
            l.setType(lyrType);
            auto actType = la["activationType"].getString();
            NeuronLayer::ActivationType actype = NeuronLayer::ActivationType::STEPPED_SIGMOID;
            if(actType == "identity"){
                actype = NeuronLayer::ActivationType::IDENTITY;
            }else if(actType == "exponential"){
                actype = NeuronLayer::ActivationType::EXPONENTIAL;
            }else if(actType == "leakyRelu"){
                actype = NeuronLayer::ActivationType::LEAKY_RELU;
            }else if(actType == "noisyRelu"){
                actype = NeuronLayer::ActivationType::NOISY_RELU;
            }else if(actType == "relu"){
                actype = NeuronLayer::ActivationType::RELU;
            }else if(actType == "sigmoid"){
                actype = NeuronLayer::ActivationType::SIGMOID;
            }else if(actType == "sinusoid"){
                actype = NeuronLayer::ActivationType::SINUSOID;
            }else if(actType == "steppedSigmoid"){
                actype = NeuronLayer::ActivationType::STEPPED_SIGMOID;
            }else if(actType == "tanh"){
                actype = NeuronLayer::ActivationType::TANH;
            }
            l.setActivationType(actype);
            auto neuronsArray = la["neurons"].getArray();
            for(auto& na:neuronsArray){
                Neuron n;
                n.setBiasWeight(na["biasWeight"].getDouble());
                n.setType(lyrType);
                auto conArray = na["connections"].getArray();
                for(auto& ca:conArray){
                    Link src(ca["src"]["layer"].getInteger(),ca["src"]["neuron"].getInteger());
                    Link dest(ca["dest"]["layer"].getInteger(),ca["dest"]["neuron"].getInteger());
                    auto weight = ca["weight"].getDouble();
                    Connection c(src,dest, weight);
                    n.addConnection(c);
                }
                l.addNeuron(n);
            }
            addLayer(l);
        }
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
        for(auto& c:getConnections()){
            auto w = c->getWeight();
            auto& src = c->getSrc();
            auto& dest = c->getDest();
            auto& nrnSrc = layers[src.layer][src.neuron];
            auto& nrnDest = layers[dest.layer][dest.neuron];
            auto output = 0.0;
            switch(nrnSrc.getType()){
                case Neuron::Type::INPUT:
                        nrnDest.addSum(nrnSrc.getSum() * w);
                    break;
                case Neuron::Type::CONTEXT:
                case Neuron::Type::HIDDEN:
                        if(nrnSrc.getType() != Neuron::Type::CONTEXT){
                            nrnSrc.addSum(layers[src.layer].getBias() * nrnSrc.getBiasWeight());
                        }
                        output = activate(layers[src.layer].getActivationType(),nrnSrc);
                        nrnSrc.setOutput(output);
                        nrnDest.addSum(output * w);
                        if(nrnDest.getType() == Neuron::Type::CONTEXT){
                            if(c->getCycles() > layers[dest.layer].getCyclesLimit()){
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
                        nrnSrc.addSum(layers[src.layer].getBias() * nrnSrc.getBiasWeight());
                        if(c->getCycles() > layers[dest.layer].getCyclesLimit()){
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
        auto& outputLayer = layers[numLayers-1];
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
    void NeuralNetwork::removeConnectionsWithDest(Link&& dest){
        connectionsCached = false;
        for(auto& l:layers){
            for(auto& n:l.getNeurons()){
                auto& conns = n.getConnections();
                auto removed = std::remove_if(std::begin(conns),std::end(conns),
                                        [&dest](const Connection& c){
                                            return (c.getDest() == dest);
                                        });
                conns.erase(removed,std::end(conns));
            }
        }
    }
    void NeuralNetwork::removeConnectionsWithSrc(Link&& src){
        connectionsCached = false;
        layers[src.layer][src.neuron].clearConnections();
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
        auto& conns = layers[src.layer][src.neuron].getConnections();
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
        auto& conns = layers[src.layer][src.neuron].getConnections();
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
    JsonBox::Value NeuralNetwork::toJson() const{
        JsonBox::Array a;
        for(auto& l:layers){
            a.push_back(l.toJson());
        }
        JsonBox::Object o;
        o["NeuralNetwork"]["layers"] = JsonBox::Value(a);
        return JsonBox::Value(o);
    }
    void NeuralNetwork::writeToFile(const std::string& filename) const{
        auto v = toJson();
        v.writeToFile(filename);
    }
    void NeuralNetwork::clear(){
        layers.clear();
        connections.clear();
        connectionsCached = false;
        mse = 0.0;
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
                return Activations::sigmoid(n.getSum()-n.getBiasWeight());
                break;
        }
    }
}
