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
    ConnectionGene::ConnectionGene(JsonBox::Object o)
    : enabled(o["enabled"].getBoolean())
    , frozen(o["frozen"].getBoolean())
    , c(o["Connection"].getObject())
    , innovationID(std::stoul(o["innovationID"].getString())){}
    bool ConnectionGene::isEnabled() const noexcept{
        return enabled;
    }
    void ConnectionGene::setEnabled(bool en) noexcept{
        enabled = en;
    }
    JsonBox::Value ConnectionGene::toJson() const noexcept{
        JsonBox::Object o;
        o["enabled"] = JsonBox::Value(enabled);
        o["frozen"] = JsonBox::Value(frozen);
        o["Connection"] = c.toJson();
        o["innovationID"] = JsonBox::Value(std::to_string(innovationID));
        return JsonBox::Value(o);
    }
    void ConnectionGene::addWeight(const double& amount) noexcept{
        c.setWeight(c.getWeight() + amount);
    }
    void ConnectionGene::setWeight(double w) noexcept{
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
    void ConnectionGene::setInnovationID(const std::size_t& id) noexcept{
        innovationID = id;
    }
    const std::size_t& ConnectionGene::getInnovationID() const noexcept{
        return innovationID;
    }
    bool ConnectionGene::operator==(const ConnectionGene& rhs) const{
        return (c.getSrc() == rhs.c.getSrc()
                && c.getDest() == rhs.c.getDest()
                && innovationID == rhs.innovationID);
    }
    bool ConnectionGene::operator!=(const ConnectionGene& rhs) const{
        return !((*this)==rhs);
    }
}