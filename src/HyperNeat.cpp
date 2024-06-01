#include <EvoAI/HyperNeat.hpp>
#include <EvoAI/Genome.hpp>

namespace EvoAI{
    SubstrateInfo::SubstrateInfo()
    : numInputs(0)
    , numHiddenLayers(0)
    , numHiddenNeurons(0)
    , numOutputs(0)
    , leo(0.5)
    , minmaxWeight(8.0)
    , bias(1.0){}
    SubstrateInfo::SubstrateInfo(std::size_t nInputs, std::size_t nHiddenLayers, 
					const std::vector<std::size_t>& nHiddenNeurons, std::size_t nOutputs, double LEO, double minMaxWeight, double Bias)
    : numInputs(nInputs)
    , numHiddenLayers(nHiddenLayers)
    , numHiddenNeurons(nHiddenNeurons)
    , numOutputs(nOutputs)
    , leo(LEO)
    , minmaxWeight(minMaxWeight)
    , bias(Bias){}
    SubstrateInfo::SubstrateInfo(JsonBox::Object o)
    : numInputs(std::stoull(o["numInputs"].getString()))
    , numHiddenLayers(std::stoull(o["numHiddenLayers"].getString()))
    , numHiddenNeurons()
    , numOutputs(std::stoull(o["numOutputs"].getString()))
    , leo(o["leo"].tryGetDouble(0.5))
    , minmaxWeight(o["minmaxWeight"].tryGetDouble(8.0))
    , bias(o["bias"].tryGetDouble(1.0)){
        for(auto& nhn:o["numHiddenNeurons"].getArray()){
            numHiddenNeurons.emplace_back(std::stoull(nhn.getString()));
        }
    }
    JsonBox::Value SubstrateInfo::toJson() const noexcept{
        JsonBox::Object o;
        o["numInputs"] = std::to_string(numInputs);
        o["numHiddenLayers"] = std::to_string(numHiddenLayers);
        JsonBox::Array a;
        a.reserve(numHiddenLayers);
        for(auto& nhn:numHiddenNeurons){
                a.emplace_back(std::to_string(nhn));
        }
        o["numHiddenNeurons"] = a;
        o["numOutputs"] = std::to_string(numOutputs);
        o["leo"] = leo;
        o["minmaxWeight"] = minmaxWeight;
        o["bias"] = bias;
        return JsonBox::Value(o);
    }
/////////////////
///// HyperNeat
/////////////////
    std::string HyperNeat::SubstrateConfigurationToString(SubstrateConfiguration sc) noexcept{
        switch(sc){
            case SubstrateConfiguration::SANDWICH: return "sandwich";
            case SubstrateConfiguration::GRID:
            default: return "grid";
        }
    }
    HyperNeat::SubstrateConfiguration HyperNeat::SubstrateConfigurationToEnum(const std::string& sc) noexcept{
        if(sc == "grid"){
            return SubstrateConfiguration::GRID;
        }else if(sc == "sandwich"){
            return SubstrateConfiguration::SANDWICH;
        }
        return SubstrateConfiguration::GRID;
    }
    HyperNeat::HyperNeat(JsonBox::Object o)
    : genome(o["Genome"].getObject())
    , substrate(o["Substrate"].getObject())
    , substrateInfo(o["substrateInfo"].getObject())
    , substrateConf(SubstrateConfigurationToEnum(o["substrateConf"].getString()))
    , isSubstrateValid(false)
    , isSubstrateReady(true){
        makeSubstrate();
    }
    HyperNeat::HyperNeat(const std::string& filename)
    : genome()
    , substrate()
    , substrateInfo()
    , substrateConf(SubstrateConfiguration::GRID)
    , isSubstrateValid(false)
    , isSubstrateReady(true){
        JsonBox::Value json;
        json.loadFromFile(filename);
        auto& v = json["HyperNeat"];
        genome = Genome(v["Genome"].getObject());
        substrate = NeuralNetwork(v["Substrate"].getObject());
        substrateInfo = SubstrateInfo(v["substrateInfo"].getObject());
        substrateConf = SubstrateConfigurationToEnum(v["substrateConf"].getString());
        makeSubstrate();
    }
    HyperNeat::HyperNeat(const SubstrateInfo& si, Genome& g, SubstrateConfiguration sc)
    : genome(g)
    , substrate(si.numInputs, si.numHiddenLayers,si.numHiddenNeurons,si.numOutputs, 1.0)
    , substrateInfo(si)
    , substrateConf(sc)
    , isSubstrateValid(false)
    , isSubstrateReady(true){
        makeSubstrate();
    }
    HyperNeat::HyperNeat(const SubstrateInfo& si, SubstrateConfiguration sc)
    : genome()
    , substrate(si.numInputs, si.numHiddenLayers,si.numHiddenNeurons,si.numOutputs, 1.0)
    , substrateInfo(si)
    , substrateConf(sc)
    , isSubstrateValid(false)
    , isSubstrateReady(true){
        switch(sc){
            case SubstrateConfiguration::SANDWICH:{
                    genome = Genome(5,2,true,true);
            }   break;
            case SubstrateConfiguration::GRID:
            default:{
                    genome = Genome(3,2,false,true);
            }    break;
        }
        makeSubstrate();
    }
    void HyperNeat::setNumInputs(std::size_t num) noexcept{
        isSubstrateReady = false;
        isSubstrateValid = false;
        substrateInfo.numInputs = num;
    }
    std::size_t HyperNeat::getNumInputs() const noexcept{
        return substrateInfo.numInputs;
    }
    void HyperNeat::setNumHiddenLayers(std::size_t num) noexcept{
        isSubstrateReady = false;
        isSubstrateValid = false;
        substrateInfo.numHiddenLayers = num;
    }
    std::size_t HyperNeat::getNumHiddenLayers() const noexcept{
        return substrateInfo.numHiddenLayers;
    }
    void HyperNeat::setNumHiddenNeurons(const std::vector<std::size_t>& num) noexcept{
        isSubstrateReady = false;
        isSubstrateValid = false;
        substrateInfo.numHiddenNeurons = num;
    }
    const std::vector<std::size_t>& HyperNeat::getNumHiddenNeurons() const noexcept{
        return substrateInfo.numHiddenNeurons;
    }
    void HyperNeat::setNumOutputs(std::size_t num) noexcept{
        isSubstrateReady = false;
        isSubstrateValid = false;
        substrateInfo.numOutputs = num;
    }
    std::size_t HyperNeat::getNumOutputs() const noexcept{
        return substrateInfo.numOutputs;
    }
    void HyperNeat::setGenome(Genome& g){
        isSubstrateValid = false;
        genome = g;
    }
    const Genome& HyperNeat::getGenome() const noexcept{
        return genome;
    }
    void HyperNeat::setSubstrate(NeuralNetwork&& subst) noexcept{
        isSubstrateReady = true;
        isSubstrateValid = true;
        substrate = std::move(subst);
    }
    NeuralNetwork& HyperNeat::getSubstrate() noexcept{
        return substrate;
    }
    void HyperNeat::makeSubstrate() noexcept{
        if(!isSubstrateValid){
            isSubstrateValid = true;
            if(!isSubstrateReady){
                substrate = NeuralNetwork(substrateInfo.numInputs, substrateInfo.numHiddenLayers,substrateInfo.numHiddenNeurons,substrateInfo.numOutputs, substrateInfo.bias);
                isSubstrateReady = true;
            }
            switch(substrateConf){
                case SubstrateConfiguration::SANDWICH:{
                    /// Genome has 5 inputs x1,x2, y1,y2, d
                    /// Genome has 2 outputs weight and leo.
                    auto nn = Genome::makePhenotype(genome);
                    for(auto x1=0u;x1<substrate.size();++x1){
                        for(auto y1=0u;y1<substrate[x1].size();++y1){
                            for(auto x2=0u;x2<substrate.size();++x2){
                                for(auto y2=0u;y2<substrate[x2].size();++y2){
                                    auto sized = static_cast<double>(substrate.size());
                                    auto d = EvoAI::distanceCenter<double>(static_cast<double>(x1),static_cast<double>(y1), sized, static_cast<double>(substrate[x1].size())) +
                                        EvoAI::distanceCenter<double>(static_cast<double>(x2),static_cast<double>(y2), sized, static_cast<double>(substrate[x2].size()));
                                    auto out = nn.forward({static_cast<double>(x1), static_cast<double>(y1), static_cast<double>(x2), static_cast<double>(y2), static_cast<double>(d)});
                                    nn.reset();
                                    if(out[1] >= substrateInfo.leo){
                                        auto weight = std::clamp(out[0], -substrateInfo.minmaxWeight, substrateInfo.minmaxWeight);
                                        auto c = Connection(Link(x1, y1), Link(x2, y2), weight);
                                        if(genome.isRecurrentAllowed()){
                                            substrate.addConnection(c);
                                        }else if(!c.isRecurrent()){
                                            substrate.addConnection(c);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }   break;
                case SubstrateConfiguration::GRID:
                default:{
                    /// Genome has 3 inputs x,y,d
                    /// Genome has 2 outputs weight, leo
                    auto& neurons = substrate.getNeurons();
                    auto size = neurons.size();
                    auto nn = Genome::makePhenotype(genome);
                    for(auto i=0u;i<size;++i){
                        for(auto j=0u;j<size;++j){
                            auto d = EvoAI::distanceCenter<double>(static_cast<double>(i), static_cast<double>(j), static_cast<double>(size), static_cast<double>(size));
                            auto out = nn.forward({static_cast<double>(i), static_cast<double>(j), d});
                            nn.reset();
                            if(out[1] >= substrateInfo.leo){
                                auto weight = std::clamp(out[0], -substrateInfo.minmaxWeight, substrateInfo.minmaxWeight);
                                auto c = Connection(substrate.getIndex(neurons[i]),substrate.getIndex(neurons[j]), weight);
                                if(genome.isRecurrentAllowed()){
                                    substrate.addConnection(c);
                                }else if(!c.isRecurrent()){
                                    substrate.addConnection(c);
                                }
                            }
                        }
                    }
                }   break;
            }
        }
    }
    void HyperNeat::setInputs(std::vector<double>&& inputs){
        if(!isSubstrateValid){
            makeSubstrate();
        }
        substrate.setInputs(std::forward<std::vector<double>>(inputs));
    }
    void HyperNeat::setInputs(const std::vector<double>& ins) noexcept{
        if(!isSubstrateValid){
            makeSubstrate();
        }
        substrate.setInputs(ins);
    }
    std::vector<double> HyperNeat::run() noexcept{
        if(!isSubstrateValid){
            makeSubstrate();
        }
        return substrate.run();
    }
    std::vector<double> HyperNeat::forward(const std::vector<double>& input) noexcept{
        setInputs(input);
        return run();
    }
    std::vector<double> HyperNeat::forward(std::vector<double>&& input) noexcept{
        setInputs(std::forward<std::vector<double>>(input));
        return run();
    }
    void HyperNeat::backward(std::vector<double>&& gradientLoss) noexcept{
        if(!isSubstrateValid){
            makeSubstrate();
        }
        substrate.backward(std::forward<std::vector<double>>(gradientLoss));
    }
    void HyperNeat::reset() noexcept{
        if(!isSubstrateValid){
            makeSubstrate();
        }
        substrate.reset();
    }
    void HyperNeat::mutate(float nodeRate, float addConnRate, float removeConnRate, float perturbWeightsRate, float enableRate, float disableRate, float actTypeRate) noexcept{
        isSubstrateValid = false;
        genome.mutate(nodeRate,addConnRate,removeConnRate, perturbWeightsRate,enableRate, disableRate, actTypeRate);
    }
    void HyperNeat::setFitness(double fitness) noexcept{
        genome.setFitness(fitness);
    }
    double HyperNeat::getFitness() const noexcept{
        return genome.getFitness();
    }
    void HyperNeat::setActivationType(std::size_t layer, Neuron::ActivationType at) noexcept{
        if(!isSubstrateValid){
            makeSubstrate();
        }
        substrate[layer].setActivationType(at);
    }
    JsonBox::Value HyperNeat::toJson() const noexcept{
        JsonBox::Object o;
        o["Genome"] = genome.toJson();
        o["Substrate"] = substrate.toJson();
        o["substrateInfo"] = substrateInfo.toJson();
        o["substrateConf"] = SubstrateConfigurationToString(substrateConf);
        return JsonBox::Value(o);
    }
    void HyperNeat::writeToFile(const std::string& filename) const noexcept{
        JsonBox::Value v;
        v["version"] = JsonBox::Value("1.0");
        v["HyperNeat"] = toJson();
#if defined NDEBUG
        v.writeToFile(filename, false, false);
#else
        v.writeToFile(filename, true, false);
#endif
    }
    bool HyperNeat::writeDotFile(const std::string& filename) noexcept{
        return getSubstrate().writeDotFile(filename);
    }
}
