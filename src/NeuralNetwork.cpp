#include <EvoAI/NeuralNetwork.hpp>
#include <fstream>

namespace EvoAI{
    NeuralNetwork::NeuralNetwork()
    : layers()
    , connections()
    , neurons()
    , connectionsCached(false)
    , neuronsCached(false)
    , globalStep(0ull)
    , lastAvgLoss(0.0){}
    NeuralNetwork::NeuralNetwork(std::size_t numInputs, std::size_t numHiddenLayers,
                                 const std::vector<std::size_t>& numNeuronsPerHiddenLayer,
                                 std::size_t numOutputs, double bias)
    : layers()
    , connections()
    , neurons()
    , connectionsCached(false)
    , neuronsCached(false)
    , globalStep(0ull)
    , lastAvgLoss(0.0){
        layers.reserve(numHiddenLayers + 2);
        layers.emplace_back(NeuronLayer(numInputs,Neuron::Type::INPUT,bias));
        for(auto i=0u;i<numHiddenLayers;++i){
            layers.emplace_back(NeuronLayer(numNeuronsPerHiddenLayer[i],Neuron::Type::HIDDEN,bias));
        }
        layers.emplace_back(NeuronLayer(numOutputs,Neuron::Type::OUTPUT,bias));
    }
    NeuralNetwork::NeuralNetwork(JsonBox::Object o)
    : layers()
    , connections()
    , neurons()
    , connectionsCached(false)
    , neuronsCached(false)
    , globalStep(std::stoull(o["globalStep"].getString()))
    , lastAvgLoss(0.0){
        auto& lyrs = o["layers"].getArray();
        layers.reserve(lyrs.size());
        for(auto& l:lyrs){
            layers.emplace_back(l.getObject());
        }
    }
    NeuralNetwork::NeuralNetwork(const std::string& filename)
    : layers()
    , connections()
    , neurons()
    , connectionsCached(false)
    , neuronsCached(false)
    , globalStep(0ull)
    , lastAvgLoss(0.0){
        JsonBox::Value v;
        v.loadFromFile(filename);
        globalStep = std::stoull(v["NeuralNetwork"]["globalStep"].getString());
        auto& layersArray = v["NeuralNetwork"]["layers"].getArray();
        layers.reserve(layersArray.size());
        for(auto& la:layersArray){
            layers.emplace_back(la.getObject());
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
        for(auto& lyr:layers){
            for(auto& n:lyr.getNeurons()){
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
        for(auto& c:getConnections()){
            auto w = c->getWeight();
            auto& src = c->getSrc();
            auto& dest = c->getDest();
            auto& nrnSrc = layers[src.layer][src.neuron];
            auto& nrnDest = layers[dest.layer][dest.neuron];
            double output = 0.0;
            switch(nrnSrc.getType()){
                case Neuron::Type::INPUT:
                        nrnSrc.setOutput(nrnSrc.getSum());
                        nrnDest.addSum(nrnSrc.getSum() * w);
                    break;
                case Neuron::Type::CONTEXT:
                case Neuron::Type::HIDDEN:
                        if(nrnSrc.getType() != Neuron::Type::CONTEXT){
                            nrnSrc.addSum(nrnSrc.getBiasWeight());
                        }
                        output = activate(nrnSrc.getActivationType(),nrnSrc);
                        nrnSrc.setOutput(output);
                        nrnDest.addSum(output * w);
                        if(nrnDest.getType() == Neuron::Type::CONTEXT){
                            if(c->getCycles() > layers[dest.layer].getCyclesLimit()){
                                nrnDest.resetContext();
                                c->setCycles(0);
                            }
                            nrnDest.setSum(nrnSrc.getSum());
                            c->setCycles(c->getCycles()+1);
                        }
                    break;
                case Neuron::Type::OUTPUT:{
                        double oldSum = nrnSrc.getSum();
                        nrnSrc.addSum(nrnSrc.getBiasWeight());
                        if(c->getCycles() > layers[dest.layer].getCyclesLimit()){
                            nrnDest.resetContext();
                            c->setCycles(0);
                        }
                        // nrnDest should be a CONTEXT neuron and nrnSrc should be Output being recorded.
                        nrnDest.setSum(nrnSrc.getSum());
                        nrnSrc.setSum(oldSum);
                        c->setCycles(c->getCycles()+1);
                }
                    break;
                default:
                    break;
            }
        }
        auto& outputLayer = layers.back();
        auto output = 0.0;
        std::vector<double> res;
        res.reserve(outputLayer.size());
        if(outputLayer.getActivationType() == Neuron::ActivationType::SOFTMAX){
            for(auto& n:outputLayer.getNeurons()){
                n.addSum(n.getBiasWeight());
                output = activate(n.getActivationType(),n);
                n.setOutput(output);
            }
            Activations::softmax(outputLayer);
            for(auto& n:outputLayer.getNeurons()){
                res.emplace_back(n.getOutput());
            }
        }else{
            for(auto& n:outputLayer.getNeurons()){
                n.addSum(n.getBiasWeight());
                output = activate(n.getActivationType(),n);
                n.setOutput(output);
                res.emplace_back(output);
            }
        }
        return res;
    }
    std::vector<double> NeuralNetwork::forward(const std::vector<double>& input) noexcept{
        setInputs(input);
        return run();
    }
    std::vector<double> NeuralNetwork::forward(std::vector<double>&& input) noexcept{
        setInputs(std::forward<std::vector<double>>(input));
        return run();
    }
    std::vector<double> NeuralNetwork::backward(std::vector<double>&& gradientLoss) noexcept{
        auto& outLayer = layers.back();
        if(outLayer.getActivationType() == Neuron::ActivationType::SOFTMAX){
            Derivatives::softmax(outLayer);
        }
        for(auto j=0u;j<outLayer.size();++j){
            outLayer[j].setGradient(derivate(outLayer[j].getActivationType(),outLayer[j]) * gradientLoss[j]);
        }
        std::for_each(std::rbegin(getConnections()),std::rend(getConnections()),
            [&](Connection* c){
                auto& src = c->getDest();
                auto& dest = c->getSrc();
                auto& nrnSrc = layers[src.layer][src.neuron];
                auto& nrnDest = layers[dest.layer][dest.neuron];
                switch(nrnSrc.getType()){
                    case Neuron::Type::OUTPUT:{
                            auto gradient = nrnSrc.getGradient();
                            nrnSrc.setBiasGradient(gradient);
                            c->addGradient(gradient * nrnDest.getOutput());
                            nrnDest.setGradient((gradient + nrnDest.getGradient()) * c->getWeight());
                    }   break;
                    case Neuron::Type::CONTEXT:
                    case Neuron::Type::HIDDEN:{
                            auto gradient = nrnSrc.getGradient() * derivate(nrnSrc.getActivationType(), nrnSrc);
                            nrnSrc.setBiasGradient(gradient);
                            c->addGradient(gradient * nrnDest.getOutput());
                            nrnDest.setGradient((gradient + nrnDest.getGradient()) * c->getWeight());
                    }   break;
                    case Neuron::Type::INPUT:
                        break;
                }
        });
        return layers[0].backward();
    }
    NeuralNetwork& NeuralNetwork::setLayers(std::vector<NeuronLayer>&& lys){
        connectionsCached = false;
        neuronsCached = false;
        layers = std::move(lys);
        return *this;
    }
    NeuralNetwork& NeuralNetwork::addNeuron(const Neuron& n, std::size_t layerIndex){
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
        return Link(0,0);
    }
    bool NeuralNetwork::setInputs(std::vector<double>&& ins) noexcept{
        auto numInputs = layers[0].size();
        if(ins.size() != numInputs){
            return false;
        }
        for(auto i=0u;i<numInputs;++i){
            layers[0][i].setSum(ins[i]);
        }
        return true;
    }
    bool NeuralNetwork::setInputs(const std::vector<double>& ins) noexcept{
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
            if((nType != Neuron::Type::INPUT) || (nType != Neuron::Type::OUTPUT)){
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
    void NeuralNetwork::removeConnectionsWithDest(const Link& dest){
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
    void NeuralNetwork::removeConnectionsWithSrc(const Link& src){
        connectionsCached = false;
        layers[src.layer][src.neuron].clearConnections();
    }
    std::vector<Connection*>& NeuralNetwork::getConnections(){
        if(connectionsCached){
            return connections;
        }
        connections.clear();
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
    std::vector<Connection*> NeuralNetwork::getParameters() noexcept{
        auto& conn = getConnections();
        connectionsCached = false;
        auto size = conn.size();
        std::vector<Connection*> params(std::move(conn));
        params.erase(std::remove_if(std::begin(params), std::end(params), 
            [](auto p){
                return p->isFrozen();
            }), std::end(params));
        connections = std::vector<Connection*>(); // restore state to valid
        connections.reserve(size);
        params.reserve(size + getNeurons().size());
        for(auto& n:getNeurons()){
            auto p = n->getBiasPtr();
            if(!p->isFrozen()){
                params.emplace_back(p);
            }
        }
        return params;
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
        return c != std::end(conns);
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
    void NeuralNetwork::resetConnections(){
        for(auto& c:getConnections()){
            c->reset();
        }
    }
    JsonBox::Value NeuralNetwork::toJson() const{
        JsonBox::Array a;
        a.reserve(layers.size());
        for(auto& l:layers){
            a.emplace_back(l.toJson());
        }
        JsonBox::Object o;
        o["globalStep"] = std::to_string(globalStep);
        o["layers"] = a;
        return o;
    }
    void NeuralNetwork::writeToFile(const std::string& filename) const{
        JsonBox::Value v;
        v["version"] = "1.0";
        v["NeuralNetwork"] = toJson();
#ifdef NDEBUG
        v.writeToFile(filename, false, false);
#else
        v.writeToFile(filename, true, false);
#endif
    }
    bool NeuralNetwork::writeDotFile(const std::string& filename) noexcept{
        auto& nn = *this;
        std::ofstream out(filename, std::ios_base::out);
        out << "digraph NeuralNetwork {\nrankdir=LR;\n";
        for(auto i=0u;i<nn.size();++i){
            for(auto j=0u;j<nn[i].size();++j){
                auto& n = nn[i][j];
                auto nType = EvoAI::Neuron::typeToString(n.getType());
                auto nAct = EvoAI::Neuron::activationTypeToString(n.getActivationType());
                auto sum = n.getSum();
                auto output = n.getOutput();
                out << "n" << i << "_" << j << " [label=\"Neuron\\n" << i << " " << j << "\\n\\nType: " << nType << "\\nActivation: " 
                    << nAct  << "\\nSum: " << sum << "\\nOutput: " << output << "\\nGrad: " << n.getGradient() << "\", shape=circle];\n";
                out << "b" << i << "_" << j << " [label=\"Bias\\nGrad: " << n.getBiasGradient() << "\", shape=square];\n";
                out << "n" << i << "_" << j << " -> b" << i << "_" << j << " [label=\"BiasWeight: " << n.getBiasWeight() << "\", style=dashed];\n";
                for(auto k=0u;k<n.size();++k){
                    auto& conn = nn[i][j][k];
                    auto& src = conn.getSrc();
                    auto& dest = conn.getDest();
                    out << "n" << src.layer << "_" << src.neuron << " -> n" << dest.layer << "_" << dest.neuron << " [label=\"Weight: " << conn.getWeight() 
                        << "\\nGrad: " << conn.getGradient() <<"\" color=\"" <<(conn.isRecurrent() ? "red":"black") << "\" , arrowhead=vee];\n";
                }
            }
        }
        out << "}\n";
        out.close();
        return true;
    }
    void NeuralNetwork::clear(){
        layers.clear();
        connections.clear();
        connectionsCached = false;
        lastAvgLoss = 0.0;
    }
    NeuronLayer& NeuralNetwork::operator[](std::size_t index){
        return layers[index];
    }
    std::pair<Neuron&, Neuron&> NeuralNetwork::operator[](Connection& conn) noexcept{
        auto& linkSrc = conn.getSrc();
        auto& linkDest = conn.getDest();
        return std::make_pair(std::ref(layers[linkSrc.layer][linkSrc.neuron]), std::ref(layers[linkDest.layer][linkDest.neuron]));
    }
    bool NeuralNetwork::operator==(const NeuralNetwork& rhs) const{
        return std::equal(std::begin(layers),std::end(layers),std::begin(rhs.layers));
    }
//private member functions
    double NeuralNetwork::derivate(Neuron::ActivationType at,const Neuron& n){
        switch(at){
            case Neuron::ActivationType::IDENTITY:
                return Derivatives::identity(n.getOutput());
            case Neuron::ActivationType::MODULUS:
                return Derivatives::modulus(n.getSum(), 2.0);
            case Neuron::ActivationType::SIGMOID:
                return Derivatives::sigmoid(n.getSum());
            case Neuron::ActivationType::SINUSOID:
                return Derivatives::sinusoid(n.getSum());
            case Neuron::ActivationType::RELU:
                return Derivatives::relu(n.getOutput());
            case Neuron::ActivationType::NOISY_RELU:
                return Derivatives::noisyRelu(n.getSum());
            case Neuron::ActivationType::LEAKY_RELU:
                return Derivatives::leakyRelu(n.getSum());
            case Neuron::ActivationType::EXPONENTIAL:
                return Derivatives::exponential(n.getSum());
            case Neuron::ActivationType::SOFTMAX:
                return n.getGradient();
            case Neuron::ActivationType::TANH:
                return Derivatives::tanh(n.getSum());
            case Neuron::ActivationType::COSINE:
                return Derivatives::cosine(n.getSum());
            case Neuron::ActivationType::TAN:
                return Derivatives::tan(n.getSum());
            case Neuron::ActivationType::GAUSSIAN:
                return Derivatives::gaussian(n.getOutput());
            case Neuron::ActivationType::STEPPED_SIGMOID:
                return Derivatives::steepenedSigmoid(n.getOutput());
            case Neuron::ActivationType::SWISH:
                return Derivatives::swish(n.getSum(), n.getBiasWeight());
            case Neuron::ActivationType::SQUARE:
                return Derivatives::square(n.getSum());
            case Neuron::ActivationType::CUBE:
                return Derivatives::cube(n.getSum());
            case Neuron::ActivationType::SOFTPLUS:
                return Derivatives::softplus(n.getSum());
            case Neuron::ActivationType::CLAMP:
                return Derivatives::clamp(n.getSum());
            case Neuron::ActivationType::INV:
                return Derivatives::inv(n.getSum());
            case Neuron::ActivationType::LOG:
                return Derivatives::log(n.getSum());
            case Neuron::ActivationType::ABS:
                return Derivatives::abs(n.getSum());
            case Neuron::ActivationType::HAT:
                return Derivatives::hat(n.getSum());
        }
        return Derivatives::sigmoid(n.getOutput());
    }
    double NeuralNetwork::activate(Neuron::ActivationType at, const Neuron& n){
        switch(at){
            case Neuron::ActivationType::IDENTITY:
                return Activations::identity(n.getSum());
            case Neuron::ActivationType::MODULUS:
                return Activations::modulus(n.getSum(), 2.0);
            case Neuron::ActivationType::TANH:
                return Activations::tanh(n.getSum());
            case Neuron::ActivationType::SINUSOID:
                return Activations::sinusoid(n.getSum());
            case Neuron::ActivationType::COSINE:
                return Activations::cosine(n.getSum());
            case Neuron::ActivationType::TAN:
                return Activations::tan(n.getSum());
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
                return Activations::identity(n.getSum());
            case Neuron::ActivationType::GAUSSIAN:
                return Activations::gaussian(n.getSum());
            case Neuron::ActivationType::STEPPED_SIGMOID:
                return Activations::steepenedSigmoid(n.getSum());
            case Neuron::ActivationType::SWISH:
                return Activations::swish(n.getSum(), n.getBiasWeight());
            case Neuron::ActivationType::SQUARE:
                return Activations::square(n.getSum());
            case Neuron::ActivationType::CUBE:
                return Activations::cube(n.getSum());
            case Neuron::ActivationType::SOFTPLUS:
                return Activations::softplus(n.getSum());
            case Neuron::ActivationType::CLAMP:
                return Activations::clamp(n.getSum());
            case Neuron::ActivationType::INV:
                return Activations::inv(n.getSum());
            case Neuron::ActivationType::LOG:
                return Activations::log(n.getSum());
            case Neuron::ActivationType::ABS:
                return Activations::abs(n.getSum());
            case Neuron::ActivationType::HAT:
                return Activations::hat(n.getSum());
        }
        return Activations::sigmoid(n.getSum());
    }
}
