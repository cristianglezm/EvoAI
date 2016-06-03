#ifndef EVOAI_CONNECTION_GENE_HPP
#define EVOAI_CONNECTION_GENE_HPP

#include <JsonBox.h>

#include <EvoAI/NodeGene.hpp>
#include <EvoAI/Connection.hpp>

#include <string>

namespace EvoAI{
    class ConnectionGene final{
        public:
            ConnectionGene(const NodeGene& src, const NodeGene& dest, double w);
            bool isEnabled() const noexcept;
            void setEnabled(bool en) noexcept;
            JsonBox::Value toJson() noexcept;
            void setWeight(double w) noexcept;
            const double& getWeight() const noexcept;
            void setFrozen(bool frzen) noexcept;
            bool isFrozen() const noexcept;
            void setInnovationID(const std::size_t& id) noexcept;
            const std::size_t& getInnovationID() const noexcept;
            bool operator==(const ConnectionGene& rhs) const;
            bool operator!=(const ConnectionGene& rhs) const;
            ~ConnectionGene() = default;
        private:
            bool enabled;
            bool frozen;
            Connection c;
            std::size_t innovationID;
    };
}

#endif // EVOAI_CONNECTION_GENE_HPP