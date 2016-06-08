#ifndef EVOAI_NODE_GENE_HPP
#define EVOAI_NODE_GENE_HPP

#include <JsonBox.h>

#include <EvoAI/Neuron.hpp>
#include <EvoAI/Utils.hpp>

#include <string>

namespace EvoAI{
    /**
     * @class NodeGene
     * @author Cristian Glez <cristian.glez.m@gmail.com>
     * @brief NodeGene for NEAT Algorithm
     */
    class NodeGene final{
        public:
            /**
             * @brief Constructor
             * Default Type = Neuron::Type::HIDDEN
             * Default ActType = Neuron::ActivationType::SIGMOID
             * @param lyrID std::size_t
             * @param nrnID std::size_t
             * @return NodeGene
             */
            NodeGene(std::size_t lyrID, std::size_t nrnID);
            /**
             * @brief Constructor
             * @param lyrID std::size_t
             * @param nrnID std::size_t
             * @param nt Neuron::Type
             * @param nat Neuron::ActivationType
             * @return NodeGene
             */
            NodeGene(std::size_t lyrID, std::size_t nrnID, Neuron::Type nt, Neuron::ActivationType nat);
            /**
             * @brief returns a json of the nodeGene
             * @return 
             */
            JsonBox::Value toJson() noexcept;
            /**
             * @brief getter for the layer id
             * @return std::size_t layer id
             */
            std::size_t getLayerID() const noexcept;
            /**
             * @brief getter for the neuron id
             * @return std::size_t neuron id
             */
            std::size_t getNeuronID() const noexcept;
            /**
             * @brief getter for Neuron::Type
             * @return Neuron::Type
             */
            Neuron::Type getNeuronType() const noexcept;
            /**
             * @brief getter for Neuron::ActivationType
             * @return Neuron::ActivationType
             */
            Neuron::ActivationType getActType() const noexcept;
            /**
             * @brief setter for innovationID
             * @param id std::size_t
             */
            void setInnovationID(const std::size_t& id) noexcept;
            /**
             * @brief getter for InnovationID
             * @return std::size_t
             */
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

namespace std{
    template<>
    struct hash<EvoAI::NodeGene>{
        using argument_type = EvoAI::NodeGene;
        using result_type = std::size_t;
        result_type operator()(argument_type const& ng) const{
            result_type const h1(std::hash<std::size_t>{}(ng.getLayerID()));
            return EvoAI::hashCombine<std::size_t>(h1,ng.getNeuronID());
        }
    };
}

#endif // EVOAI_NODE_GENE_HPP