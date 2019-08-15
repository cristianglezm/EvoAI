#include <EvoAI/HyperNeat.hpp>

namespace EvoAI{
    SubstrateInfo::SubstrateInfo()
    : numInputs(0)
    , numHiddenLayers(0)
    , numHiddenNeurons(0)
    , numOutputs(0){}
    SubstrateInfo::SubstrateInfo(const std::size_t& numInputs, const std::size_t& numHiddenLayers, const std::size_t& numHiddenNeurons, const std::size_t& numOutputs)
    : numInputs(numInputs)
    , numHiddenLayers(numHiddenLayers)
    , numHiddenNeurons(numHiddenNeurons)
    , numOutputs(numOutputs){}
    SubstrateInfo::SubstrateInfo(JsonBox::Object o)
    : numInputs(std::stoull(o["numInputs"].getString()))
    , numHiddenLayers(std::stoull(o["numHiddenLayers"].getString()))
    , numHiddenNeurons(std::stoull(o["numHiddenNeurons"].getString()))
    , numOutputs(std::stoull(o["numOutputs"].getString())){}
    JsonBox::Value SubstrateInfo::toJson() const noexcept{
        JsonBox::Object o;
        o["numInputs"] = std::to_string(numInputs);
        o["numHiddenLayers"] = std::to_string(numHiddenLayers);
        o["numHiddenNeurons"] = std::to_string(numHiddenNeurons);
        o["numOutputs"] = std::to_string(numOutputs);
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
                    genome = Genome(3,2,true,true);
            }    break;
        }
        makeSubstrate();
    }
    void HyperNeat::setNumInputs(const std::size_t& num) noexcept{
        isSubstrateReady = false;
        isSubstrateValid = false;
        substrateInfo.numInputs = num;
    }
    const std::size_t& HyperNeat::getNumInputs() const noexcept{
        return substrateInfo.numInputs;
    }
    void HyperNeat::setNumHiddenLayers(const std::size_t& num) noexcept{
        isSubstrateReady = false;
        isSubstrateValid = false;
        substrateInfo.numHiddenLayers = num;
    }
    const std::size_t& HyperNeat::getNumHiddenLayers() const noexcept{
        return substrateInfo.numHiddenLayers;
    }
    void HyperNeat::setNumHiddenNeurons(const std::size_t& num) noexcept{
        isSubstrateReady = false;
        isSubstrateValid = false;
        substrateInfo.numHiddenNeurons = num;
    }
    const std::size_t& HyperNeat::getNumHiddenNeurons() const noexcept{
        return substrateInfo.numHiddenNeurons;
    }
    void HyperNeat::setNumOutputs(const std::size_t& num) noexcept{
        isSubstrateReady = false;
        isSubstrateValid = false;
        substrateInfo.numOutputs = num;
    }
    const std::size_t& HyperNeat::getNumOutputs() const noexcept{
        return substrateInfo.numOutputs;
    }
    void HyperNeat::setGenome(Genome& g){
        isSubstrateValid = false;
        genome = g;
    }
    const Genome& HyperNeat::getGenome() const noexcept{
        return genome;
    }
    NeuralNetwork& HyperNeat::getSubstrate() noexcept{
        return substrate;
    }
    void HyperNeat::makeSubstrate() noexcept{
        if(!isSubstrateValid){
            isSubstrateValid = true;
            if(!isSubstrateReady){
                substrate = NeuralNetwork(substrateInfo.numInputs, substrateInfo.numHiddenLayers,substrateInfo.numHiddenNeurons,substrateInfo.numOutputs, 1.0);
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
                                    auto d = EvoAI::distanceCenter<int>(x1,y1,substrate.size(),substrate[x1].size()) +
                                        EvoAI::distanceCenter<int>(x2,y2,substrate.size(),substrate[x2].size());
                                    nn->setInputs({static_cast<double>(x1), static_cast<double>(y1), static_cast<double>(x2), static_cast<double>(y2), static_cast<double>(d)});
                                    auto out = nn->run();
                                    nn->reset();
                                    if(std::fabs(out[1]) > 0.0){
                                        auto weight = out[0];
                                        if(weight < -8.0 || weight > 8.0 || weight == 0.0){
                                            weight = randomGen.random(-1.0,1.0);
                                        }
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
                            auto d = EvoAI::distanceCenter<int>(i,j,size,size);
                            nn->setInputs({static_cast<double>(i), static_cast<double>(j), static_cast<double>(d)});
                            auto out = nn->run();
                            nn->reset();
                            if(std::fabs(out[1]) > 0.0){
                                auto weight = out[0];
                                if(weight < -8.0 || weight > 8.0 || weight == 0.0){
                                    weight = randomGen.random(-1.0,1.0);
                                }
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
        substrate.setInputs(std::move(inputs));
    }
    std::vector<double> HyperNeat::run() noexcept{
        if(!isSubstrateValid){
            makeSubstrate();
        }
        return substrate.run();
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
    void HyperNeat::setFitness(const double& fitness) noexcept{
        genome.setFitness(fitness);
    }
    const double& HyperNeat::getFitness() const noexcept{
        return genome.getFitness();
    }
    void HyperNeat::setActivationType(const std::size_t& layer, Neuron::ActivationType at) noexcept{
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
        v.writeToFile(filename);
    }
}