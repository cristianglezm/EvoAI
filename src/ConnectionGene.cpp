#include <EvoAI/ConnectionGene.hpp>

namespace EvoAI{
    ConnectionGene::ConnectionGene(const NodeGene& src, const NodeGene& dest, double w)
    : enabled(true)
    , frozen(false)
    , c(Link(src.getLayerID(), src.getNeuronID()), Link(dest.getLayerID(), dest.getNeuronID()), w)
    , innovationID(0){
        innovationID = std::hash<ConnectionGene>{}(*this);
    }
    ConnectionGene::ConnectionGene(const Link& src, const Link& dest, double w)
    : enabled(true)
    , frozen(false)
    , c(src, dest, w)
    , innovationID(0){
        innovationID = std::hash<ConnectionGene>{}(*this);
    }
    ConnectionGene::ConnectionGene(const ConnectionGene& rhs) noexcept
    : enabled(rhs.enabled)
    , frozen(rhs.frozen)
    , c(rhs.c)
    , innovationID(rhs.innovationID){}
    ConnectionGene::ConnectionGene(ConnectionGene&& rhs) noexcept
    : enabled(rhs.enabled)
    , frozen(rhs.frozen)
    , c(rhs.c)
    , innovationID(rhs.innovationID){}
    ConnectionGene::ConnectionGene(JsonBox::Object o)
    : enabled(o["enabled"].getBoolean())
    , frozen(o["frozen"].getBoolean())
    , c(o["Connection"].getObject())
    , innovationID(std::hash<ConnectionGene>{}(*this)){}
    bool ConnectionGene::isEnabled() const noexcept{
        return enabled;
    }
    void ConnectionGene::setEnabled(bool en) noexcept{
        enabled = en;
    }
    std::string ConnectionGene::toString(const std::string& delimiter) const noexcept{
        std::string str;
        str += "ID: " + std::to_string(innovationID) + delimiter + "conn: [" + std::to_string(getSrc().layer) + ", " + std::to_string(getSrc().neuron)
            + "] -> [" + std::to_string(getDest().layer) + ", " + std::to_string(getDest().neuron) + "]" + delimiter + "w: " + std::to_string(getWeight())
            + delimiter + "enabled: " + (enabled ? "true":"false");
        return str;
    }
    JsonBox::Value ConnectionGene::toJson() const noexcept{
        JsonBox::Object o;
        o["enabled"] = JsonBox::Value(enabled);
        o["frozen"] = JsonBox::Value(frozen);
        o["Connection"] = c.toJson();
        return JsonBox::Value(o);
    }
    void ConnectionGene::addWeight(const double& amount) noexcept{
        c.setWeight(c.getWeight() + amount);
    }
    void ConnectionGene::setWeight(const double& w) noexcept{
        c.setWeight(w);
    }
    const double& ConnectionGene::getWeight() const noexcept{
        return c.getWeight();
    }
    const Link& ConnectionGene::getSrc() const noexcept{
        return c.getSrc();
    }
    const Link& ConnectionGene::getDest() const noexcept{
        return c.getDest();
    }
    const Connection& ConnectionGene::getConnection() const noexcept{
        return c;
    }
    void ConnectionGene::setFrozen(bool frzen) noexcept{
        frozen = frzen;
    }
    bool ConnectionGene::isFrozen() const noexcept{
        return frozen;
    }
    const std::size_t& ConnectionGene::getInnovationID() const noexcept{
        return innovationID;
    }
    bool ConnectionGene::operator==(const ConnectionGene& rhs) const{
        return innovationID == rhs.innovationID;
    }
    bool ConnectionGene::operator!=(const ConnectionGene& rhs) const{
        return !((*this)==rhs);
    }
    void ConnectionGene::operator=(const ConnectionGene& rhs) noexcept{
        enabled = rhs.enabled;
        frozen = rhs.frozen;
        c = rhs.c;
        innovationID = rhs.innovationID;
    }
    void ConnectionGene::operator=(ConnectionGene&& rhs) noexcept{
        enabled = rhs.enabled;
        frozen = rhs.frozen;
        c = rhs.c;
        innovationID= rhs.innovationID;
    }
}