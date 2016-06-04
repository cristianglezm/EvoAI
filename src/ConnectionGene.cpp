#include <EvoAI/ConnectionGene.hpp>

namespace EvoAI{
    ConnectionGene::ConnectionGene(const NodeGene& src, const NodeGene& dest, double w)
    : enabled(true)
    , frozen(false)
    , c(Link(src.getLayerID(), src.getNeuronID()), Link(dest.getLayerID(), dest.getNeuronID()), w)
    , innovationID(0){}
    bool ConnectionGene::isEnabled() const noexcept{
        return enabled;
    }
    void ConnectionGene::setEnabled(bool en) noexcept{
        enabled = en;
    }
    JsonBox::Value ConnectionGene::toJson() noexcept{
        JsonBox::Object o;
        o["enabled"] = JsonBox::Value(enabled ? "True":"False");
        o["Connection"] = c.toJson();
        o["innovationID"] = JsonBox::Value(std::to_string(innovationID));
        return JsonBox::Value(o);
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
        return (enabled == rhs.enabled
                && c == rhs.c
                && innovationID == rhs.innovationID);
    }
    bool ConnectionGene::operator!=(const ConnectionGene& rhs) const{
        return !((*this)==rhs);
    }
}