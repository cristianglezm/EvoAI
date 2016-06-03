#ifndef EVOAI_NODE_GENE_HPP
#define EVOAI_NODE_GENE_HPP

#include <JsonBox.h>

#include <EvoAI/Neuron.hpp>

#include <string>

namespace EvoAI{
    class NodeGene final{
        public:
            NodeGene(std::size_t lyrID, std::size_t nrnID, Neuron::Type nt, Neuron::ActivationType nat);
            JsonBox::Value toJson() noexcept;
            std::size_t getLayerID() const noexcept;
            std::size_t getNeuronID() const noexcept;
            Neuron::Type getNeuronType() const noexcept;
            Neuron::ActivationType getActType() const noexcept;
            void setInnovationID(const std::size_t& id) noexcept;
            const std::size_t& getInnovationID() const noexcept;
            bool operator==(const NodeGene& rhs) const;
            bool operator!=(const NodeGene& rhs) const;
            ~NodeGene() = default;
        private:
            std::size_t layerID;
            std::size_t neuronID;
            Neuron::Type nrnType;
            Neuron::ActivationType actType;
            std::size_t innovationID;
    };
}

#endif // EVOAI_NODE_GENE_HPP