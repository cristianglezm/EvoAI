#include <EvoAI/NeuralNetwork.hpp>

namespace EvoAI{
    NeuralNetwork::NeuralNetwork()
    : layers()
    , connections()
    , neurons()
    , connectionsCached(false)
    , neuronsCached(false)
    , mse(0.0){}
    NeuralNetwork::NeuralNetwork(const std::size_t& numInputs, const std::size_t& numHiddenLayers,
                                 const std::size_t& numNeuronsPerHiddenLayer,
                                 const std::size_t& numOutputs, const double& bias)
    : layers()
    , connections()
    , neurons()
    , connectionsCached(false)
    , neuronsCached(false)
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
    , neurons()
    , connectionsCached(false)
    , neuronsCached(false)
    , mse(0.0){
        JsonBox::Value v;
        v.loadFromFile(filename);
        auto layersArray = v["NeuralNetwork"]["layers"].getArray();
        //build layer
        for(auto& la:layersArray){
            NeuronLayer l;
            l.setCyclesLimit(la["cyclesLimit"].getInteger());
            l.setBias(la["bias"].getDouble());
            auto type = la["neuronType"].getString();
            l.setType(Neuron::typeToEnum(type));
            auto actType = la["activationType"].getString();
            l.setActivationType(Neuron::activationTypeToEnum(actType));
            // build neurons
            auto neuronsArray = la["neurons"].getArray();
            for(auto& na:neuronsArray){
                Neuron n;
                n.setBiasWeight(na["biasWeight"].getDouble());
                auto nType = na["type"].getString();
                n.setType(Neuron::typeToEnum(nType));
                auto nActType = na["activationType"].getString();
                n.setActivationType(Neuron::activationTypeToEnum(nActType));
                //build connections
                auto conArray = na["connections"].getArray();
                for(auto& ca:conArray){
                    Link src(ca["src"]["layer"].getInteger(), ca["src"]["neuron"].getInteger());
                    Link dest(ca["dest"]["layer"].getInteger(), ca["dest"]["neuron"].getInteger());
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
        connectionsCached = false;
        neuronsCached = false;
        return *this;
    }
    bool NeuralNetwork::removeLayer(const NeuronLayer& l){
        auto lyrIndex = 0u;
        connectionsCached = false;
        neuronsCached = false;
        for(auto i=0u;i<layers.size();++i){
            if(l == layers[i]){
                lyrIndex = i;
            }
        }
        for(auto& l:layers){
            for(auto& n:l.getNeurons()){
                auto& conns = n.getConnections();
                auto removed = std::remove_if(std::begin(conns),std::end(conns),
                                        [&lyrIndex](const Connection& c){
                                            return (c.getDest().layer == lyrIndex);
                                        });
                conns.erase(removed,std::end(conns));
            }
        }
        auto lyrRemoved = std::remove_if(std::begin(layers),std::end(layers),
                                      [&l](const NeuronLayer& rl){
                                        return (l == rl);
                                      });
        layers.erase(lyrRemoved,std::end(layers));
        return (lyrRemoved != std::end(layers));
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
                        output = activate(nrnSrc.getActivationType(),nrnSrc);
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
            output = activate(n.getActivationType(),n);
            n.setOutput(output);
            res.emplace_back(output);
        }
        return std::move(res);
    }
    void NeuralNetwork::train(std::vector<std::vector<double>>&& inputs,std::vector<std::vector<double>>&& expectedOutputs,
                                                                        const double& learningRate, const double& momentum, const int& epoch){
        if(inputs.size() != expectedOutputs.size()){
            throw std::runtime_error("train() : inputs and expectedOutputs should have the same size.");
        }
        auto batchSize = inputs.size();
        auto totalError = 0.0;
        for(auto e=0;e<epoch;++e){
            for(auto i=0u;i<batchSize;++i){
                auto input(inputs[i]);
                setInputs(std::move(input));
                auto outputs = run();
                auto& outLayer = layers[layers.size()-1];
                for(auto j=0u;j<outLayer.size();++j){
                    auto error = expectedOutputs[i][j] - outputs[j];
                    totalError += std::pow(error,2);
                    auto delta = error * derivate(outLayer[j].getActivationType(),outLayer[j]);
                    outLayer[j].setDelta(delta);
                }
                std::for_each(std::rbegin(getConnections()),std::rend(getConnections()),
                    [this,&learningRate](Connection* c){
                        auto& src = c->getDest();
                        auto& dest = c->getSrc();
                        auto& nrnSrc = layers[src.layer][src.neuron];
                        auto& nrnDest = layers[dest.layer][dest.neuron];
                        switch(nrnSrc.getType()){
                            case Neuron::Type::OUTPUT:{
                                    auto sum = 0.0;
                                    for(auto& dCon:nrnDest.getConnections()){
                                        sum += dCon.getWeight() * layers[dCon.getDest().layer][dCon.getDest().neuron].getDelta();
                                    }
                                    auto delta = derivate(nrnDest.getActivationType(),nrnDest) * sum;
                                    nrnDest.setDelta(delta);
                                    auto grad = nrnDest.getOutput() * nrnSrc.getDelta();
                                    c->addGradient(grad);
                            }   break;
                            case Neuron::Type::CONTEXT:
                            case Neuron::Type::HIDDEN:{
                                    if(nrnDest.getType() != Neuron::Type::INPUT){
                                        auto sum = 0.0;
                                        for(auto& dCon:nrnDest.getConnections()){
                                            sum += dCon.getWeight() * layers[dCon.getDest().layer][dCon.getDest().neuron].getDelta();
                                        }
                                        auto delta = sum * derivate(nrnDest.getActivationType(),nrnDest);
                                        nrnDest.setDelta(delta);
                                        auto grad = nrnDest.getOutput() * nrnSrc.getDelta();
                                        c->addGradient(grad);
                                    }else{
                                        auto sum = 0.0;
                                        for(auto& dCon:nrnDest.getConnections()){
                                            sum += dCon.getWeight() * layers[dCon.getDest().layer][dCon.getDest().neuron].getDelta();
                                        }
                                        auto delta = sum * derivate(nrnDest.getActivationType(),nrnDest);
                                        nrnDest.setDelta(delta);
                                        auto grad = nrnDest.getSum() * nrnSrc.getDelta();
                                        c->addGradient(grad);
                                    }
                            }   break;
                            case Neuron::Type::INPUT:
                                break;
                        }
                    });
                    reset();
            }
            totalError /= (batchSize * expectedOutputs[0].size());
            mse = totalError;
            totalError = 0.0;
            auto conSize = getConnections().size();
            for(auto index=0u;index<conSize;++index){
                auto c = connections[index];
                if(!c->isRecurrent()){
                    auto oldWDelta = 0.0;
                    if(index > 0){
                        oldWDelta = (*connections[index-1]).getDelta();
                    }
                    c->setDelta(learningRate * c->getGradient() + momentum * oldWDelta);
                    c->setWeight(c->getWeight() + c->getDelta());
                }
            }
            resetConnections();
        }
    }
    NeuralNetwork& NeuralNetwork::setLayers(std::vector<NeuronLayer>&& lys){
        connectionsCached = false;
        neuronsCached = false;
        layers = std::move(lys);
        return *this;
    }
    NeuralNetwork& NeuralNetwork::addNeuron(const Neuron& n, const std::size_t& layerIndex){
        neuronsCached = false;
        connectionsCached = false;
        layers[layerIndex].addNeuron(n);
        return *this;
    }
    bool NeuralNetwork::removeNeuron(Neuron* n){
        auto link = getIndex(n);
        removeConnectionsWithDest(link);
        auto isRemoved = layers[link.layer].removeNeuron(n);
        for(auto& c:getConnections()){
            if(c->getDest().layer == link.layer){
                if(link.neuron != layers[link.layer].size()){
                    if(c->getDest().neuron > link.neuron){
                        auto& dest = c->getDest();
                        c->setDest(Link(dest.layer,dest.neuron-1));
                    }
                }
            }
            if(c->getSrc().layer == link.layer){
                if(link.neuron != layers[link.layer].size()){
                    if(c->getSrc().neuron > link.neuron){
                        auto& src = c->getSrc();
                        c->setSrc(Link(src.layer,src.neuron-1));
                    }
                }
            }
        }
        connectionsCached = false;
        neuronsCached = false;
        return isRemoved;
    }
    Link NeuralNetwork::getIndex(Neuron* n) const{
        if(n->hasConnections()){
            return n->getConnections()[0].getSrc();
        }
        for(auto i=0u;i<layers.size();++i){
            for(auto j=0u;j<layers[i].size();++j){
                if(layers[i][j] == (*n)){
                    return Link(i,j);
                }
            }
        }
        return Link(-1,-1);
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
        if(c.isRecurrent()){
            auto nType = layers[c.getDest().layer][c.getDest().neuron].getType();
            if( nType != Neuron::Type::INPUT || nType != Neuron::Type::OUTPUT){
                layers[c.getDest().layer][c.getDest().neuron].setType(Neuron::Type::CONTEXT);
            }
        }
        layers[c.getSrc().layer].addConnection(c);
        connectionsCached = false;
        return *this;
    }
    bool NeuralNetwork::removeConnection(Connection& c){
        connectionsCached = false;
        return layers[c.getSrc().layer].removeConnection(c);
    }
    void NeuralNetwork::removeConnectionsWithDest(Link dest){
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
    void NeuralNetwork::removeConnectionsWithSrc(Link src){
        connectionsCached = false;
        layers[src.layer][src.neuron].clearConnections();
    }
    std::vector<Connection*>& NeuralNetwork::getConnections(){
        if(connectionsCached){
            return connections;
        }else{
            connections.clear();
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
    std::vector<Neuron*>& NeuralNetwork::getNeurons(){
        if(neuronsCached){
            return neurons;
        }else{
            neurons.clear();
        }
        for(auto& l:layers){
            for(auto& n:l.getNeurons()){
                neurons.emplace_back(&n);
            }
        }
        neuronsCached = true;
        return neurons;
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
        o["version"] = JsonBox::Value("1.0");
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
    bool NeuralNetwork::operator==(const NeuralNetwork& rhs) const{
        return std::equal(std::begin(layers),std::end(layers),std::begin(rhs.layers));
    }
//private member functions
    const double NeuralNetwork::derivate(Neuron::ActivationType at,const Neuron& n){
        switch(at){
            case Neuron::ActivationType::IDENTITY:
                return Derivatives::identity(n.getSum());
            case Neuron::ActivationType::SIGMOID:
                return Derivatives::sigmoid(n.getSum());
            case Neuron::ActivationType::SINUSOID:
                return Derivatives::sinusoid(n.getSum());
            case Neuron::ActivationType::RELU:
                return Derivatives::relu(n.getSum());
            case Neuron::ActivationType::TANH:
                return Derivatives::tanh(n.getSum());
            case Neuron::ActivationType::GAUSSIAN:
                return Derivatives::gaussian(n.getSum());
            case Neuron::ActivationType::STEPPED_SIGMOID:
                return Derivatives::sigmoid(n.getSum()-0.5);
        }
        return Derivatives::sigmoid(n.getSum()-0.5);
    }
    const double NeuralNetwork::activate(Neuron::ActivationType at, const Neuron& n){
        switch(at){
            case Neuron::ActivationType::IDENTITY:
                return Activations::identity(n.getSum());
            case Neuron::ActivationType::MODULUS:
                return Activations::modulus(n.getSum());
            case Neuron::ActivationType::TANH:
                return Activations::tanh(n.getSum());
            case Neuron::ActivationType::SINUSOID:
                return Activations::sinusoid(n.getSum());
            case Neuron::ActivationType::SIGMOID:
                return Activations::sigmoid(n.getSum());
            case Neuron::ActivationType::RELU:
                return Activations::relu(n.getSum());
            case Neuron::ActivationType::NOISY_RELU:
                return Activations::noisyRelu(n.getSum());
            case Neuron::ActivationType::LEAKY_RELU:
                return Activations::leakyRelu(n.getSum());
            case Neuron::ActivationType::EXPONENTIAL:
                return Activations::exponential(n.getSum());
            case Neuron::ActivationType::SOFTMAX:
                return Activations::softmax(n.getSum(),*this);
            case Neuron::ActivationType::GAUSSIAN:
                return Activations::gaussian(n.getSum());
            case Neuron::ActivationType::STEPPED_SIGMOID:
                 return Activations::sigmoid(n.getSum()-0.5);
        }
        return Activations::sigmoid(n.getSum()-0.5);
    }
    void NeuralNetwork::resetConnections(){
        for(auto& c:getConnections()){
            c->reset();
        }
    }
}