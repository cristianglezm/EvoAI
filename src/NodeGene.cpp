#include <EvoAI/NodeGene.hpp>

#include <algorithm>

namespace EvoAI{
    NodeGene::NodeGene(const std::size_t& lyrID, const std::size_t& nrnID)
    : layerID(lyrID)
    , neuronID(nrnID)
    , biasWeight(1.0)
    , nrnType(Neuron::Type::HIDDEN)
    , actType(Neuron::ActivationType::SIGMOID)
    , innovationID(0){
        innovationID = std::hash<NodeGene>{}(*this);
    }
    NodeGene::NodeGene(const std::size_t& lyrID, const std::size_t& nrnID, Neuron::Type nt, Neuron::ActivationType nat)
    : layerID(lyrID)
    , neuronID(nrnID)
    , biasWeight(1.0)
    , nrnType(nt)
    , actType(nat)
    , innovationID(0){
        innovationID = std::hash<NodeGene>{}(*this);
    }
    NodeGene::NodeGene(const NodeGene& rhs) noexcept
    : layerID(rhs.layerID)
    , neuronID(rhs.neuronID)
    , biasWeight(rhs.biasWeight)
    , nrnType(rhs.nrnType)
    , actType(rhs.actType)
    , innovationID(rhs.innovationID){}
    NodeGene::NodeGene(NodeGene&& rhs) noexcept
    : layerID(rhs.layerID)
    , neuronID(rhs.neuronID)
    , biasWeight(rhs.biasWeight)
    , nrnType(rhs.nrnType)
    , actType(rhs.actType)
    , innovationID(rhs.innovationID){}
    NodeGene::NodeGene(JsonBox::Object o)
    : layerID(std::stoull(o["layerID"].getString()))
    , neuronID(std::stoull(o["neuronID"].getString()))
    , biasWeight(o["biasWeight"].getDouble())
    , nrnType(Neuron::typeToEnum(o["nrnType"].getString()))
    , actType(Neuron::activationTypeToEnum(o["actType"].getString()))
    , innovationID(std::hash<NodeGene>{}(*this)){}
    JsonBox::Value NodeGene::toJson() const noexcept{
        JsonBox::Object o;
        o["layerID"] = JsonBox::Value(std::to_string(layerID));
        o["neuronID"] = JsonBox::Value(std::to_string(neuronID));
        o["biasWeight"] = JsonBox::Value(biasWeight);
        o["nrnType"] = JsonBox::Value(Neuron::typeToString(nrnType));
        o["actType"] = JsonBox::Value(Neuron::activationTypeToString(actType));
        return JsonBox::Value(o);
    }
    std::size_t NodeGene::getLayerID() const noexcept{
        return layerID;
    }
    std::size_t NodeGene::getNeuronID() const noexcept{
        return neuronID;
    }
    void NodeGene::setNeuronType(Neuron::Type t) noexcept{
        nrnType = t;
    }
    Neuron::Type NodeGene::getNeuronType() const noexcept{
        return nrnType;
    }
    void NodeGene::setActType(Neuron::ActivationType at) noexcept{
        actType = at;
    }
    Neuron::ActivationType NodeGene::getActType() const noexcept{
        return actType;
    }
    const std::size_t& NodeGene::getInnovationID() const noexcept{
        return innovationID;
    }
    void NodeGene::addBias(const double& amount) noexcept{
        biasWeight += amount;
    }
    void NodeGene::setBias(const double& bw) noexcept{
        biasWeight = bw;
    }
    const double& NodeGene::getBias() const noexcept{
        return biasWeight;
    }
    bool NodeGene::operator==(const NodeGene& rhs) const{
        return innovationID == rhs.innovationID;
    }
    bool NodeGene::operator!=(const NodeGene& rhs) const{
        return !((*this)==rhs);
    }
    void NodeGene::operator=(const NodeGene& rhs) noexcept{
        layerID = rhs.layerID;
        neuronID = rhs.neuronID;
        biasWeight = rhs.biasWeight;
        nrnType = rhs.nrnType;
        actType = rhs.actType;
        innovationID = rhs.innovationID;
    }
    void NodeGene::operator=(NodeGene&& rhs) noexcept{
        layerID = rhs.layerID;
        neuronID = rhs.neuronID;
        biasWeight = rhs.biasWeight;
        nrnType = rhs.nrnType;
        actType = rhs.actType;
        innovationID = rhs.innovationID;
    }
}