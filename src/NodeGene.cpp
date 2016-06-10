#include <EvoAI/NodeGene.hpp>

#include <algorithm>

namespace EvoAI{
    NodeGene::NodeGene(std::size_t lyrID, std::size_t nrnID)
    : layerID(lyrID)
    , neuronID(nrnID)
    , biasWeight(1.0)
    , nrnType(Neuron::Type::HIDDEN)
    , actType(Neuron::ActivationType::SIGMOID)
    , innovationID(0){
        innovationID = std::hash<NodeGene>{}(*this);
    }
    NodeGene::NodeGene(std::size_t lyrID, std::size_t nrnID, Neuron::Type nt, Neuron::ActivationType nat)
    : layerID(lyrID)
    , neuronID(nrnID)
    , biasWeight(1.0)
    , nrnType(nt)
    , actType(nat)
    , innovationID(0){
        innovationID = std::hash<NodeGene>{}(*this);
    }
    NodeGene::NodeGene(JsonBox::Object o)
    : layerID(std::stoul(o["layerID"].getString()))
    , neuronID(std::stoul(o["neuronID"].getString()))
    , biasWeight(o["biasWeight"].getDouble())
    , nrnType(Neuron::typeToEnum(o["nrnType"].getString()))
    , actType(Neuron::activationTypeToEnum(o["actType"].getString()))
    , innovationID(std::stoul(o["innovationID"].getString())){}
    JsonBox::Value NodeGene::toJson() const noexcept{
        JsonBox::Object o;
        o["layerID"] = JsonBox::Value(std::to_string(layerID));
        o["neuronID"] = JsonBox::Value(std::to_string(neuronID));
        o["biasWeight"] = JsonBox::Value(biasWeight);
        o["nrnType"] = JsonBox::Value(Neuron::typeToString(nrnType));
        o["actType"] = JsonBox::Value(Neuron::activationTypeToString(actType));
        o["innovationID"] = JsonBox::Value(std::to_string(innovationID));
        return JsonBox::Value(o);
    }
    std::size_t NodeGene::getLayerID() const noexcept{
        return layerID;
    }
    std::size_t NodeGene::getNeuronID() const noexcept{
        return neuronID;
    }
    Neuron::Type NodeGene::getNeuronType() const noexcept{
        return nrnType;
    }
    Neuron::ActivationType NodeGene::getActType() const noexcept{
        return actType;
    }
    void NodeGene::setInnovationID(const std::size_t& id) noexcept{
        innovationID = id;
    }
    const std::size_t& NodeGene::getInnovationID() const noexcept{
        return innovationID;
    }
    void NodeGene::setBias(const double& bw) noexcept{
        biasWeight = bw;
    }
    const double& NodeGene::getBias() const noexcept{
        return biasWeight;
    }
    bool NodeGene::operator==(const NodeGene& rhs) const{
        return (layerID == rhs.layerID
                && neuronID == rhs.neuronID
                && innovationID == rhs.innovationID);
    }
    bool NodeGene::operator!=(const NodeGene& rhs) const{
        return !((*this)==rhs);
    }
}