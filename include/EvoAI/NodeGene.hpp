#ifndef EVOAI_NODE_GENE_HPP
#define EVOAI_NODE_GENE_HPP

#include <JsonBox.h>

#include <EvoAI/Neuron.hpp>
#include <EvoAI/Export.hpp>
#include <EvoAI/Utils.hpp>

#include <string>
#include <cassert>

namespace EvoAI{
    /**
     * @class NodeGene
     * @author Cristian Glez <cristian.glez.m@gmail.com>
     * @brief NodeGene for NEAT Algorithm
     */
    class EvoAI_API NodeGene final{
        public:
            /**
             * @brief Constructor
             * Default Type = Neuron::Type::HIDDEN
             * Default ActType = Neuron::ActivationType::SIGMOID
             * @param lyrID std::size_t
             * @param nrnID std::size_t
             * @return NodeGene
             */
            NodeGene(const std::size_t& lyrID, const std::size_t& nrnID);
            /**
             * @brief Constructor
             * @param lyrID std::size_t
             * @param nrnID std::size_t
             * @param nt Neuron::Type
             * @param nat Neuron::ActivationType
             * @return NodeGene
             */
            NodeGene(const std::size_t& lyrID, const std::size_t& nrnID, Neuron::Type nt, Neuron::ActivationType nat);
            /**
             *  @brief copy constructor
             *  
             *  @param [in] rhs NodeGene
             */
            NodeGene(const NodeGene& rhs) noexcept;
            /**
             *  @brief move constructor
             *  
             *  @param [in] rhs NodeGene
             */
            NodeGene(NodeGene&& rhs) noexcept;
            /**
             * @brief Constructor with a json object
             * @param o JsonBox::Object&
             */ 
            NodeGene(JsonBox::Object o);
            /**
             * @brief returns a json of the nodeGene
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
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
             * @brief sets a new Neuron::Type
             * @param t Neuron::Type
             */
            void setNeuronType(Neuron::Type t) noexcept;
            /**
             * @brief getter for Neuron::Type
             * @return Neuron::Type
             */
            Neuron::Type getNeuronType() const noexcept;
            /**
             * @brief sets a new Neuron::ActivationType
             * @param at Neuron::ActivationType
             */
            void setActType(Neuron::ActivationType at) noexcept;
            /**
             * @brief getter for Neuron::ActivationType
             * @return Neuron::ActivationType
             */
            Neuron::ActivationType getActType() const noexcept;
            /**
             * @brief getter for InnovationID
             * @return std::size_t
             */
            const std::size_t& getInnovationID() const noexcept;
            /**
             * @brief adds the amount to current bias.
             * @param amount
             */
            void addBias(const double& amount) noexcept;
            /**
             * @brief setter for the node bias.
             * @param bw bias weight
             */
            void setBias(const double& bw) noexcept;
            /**
             * @brief getter for bias
             * @return const double& bias
             */
            const double& getBias() const noexcept;
            bool operator==(const NodeGene& rhs) const;
            bool operator!=(const NodeGene& rhs) const;
            void operator=(const NodeGene& rhs) noexcept;
            void operator=(NodeGene&& rhs) noexcept;
            constexpr bool operator<(const NodeGene& rhs) const noexcept;
            constexpr bool operator>(const NodeGene& rhs) const noexcept;
            ~NodeGene() = default;
        private:
            std::size_t layerID;
            std::size_t neuronID;
            double biasWeight;
            Neuron::Type nrnType;
            Neuron::ActivationType actType;
            std::size_t innovationID;
    };
    constexpr bool NodeGene::operator<(const NodeGene& rhs) const noexcept{
        return innovationID < rhs.innovationID;
    }
    constexpr bool NodeGene::operator>(const NodeGene& rhs) const noexcept{
        return !((*this)<rhs);
    }
}

namespace std{
    /**
     * @brief specialization of std::hash for EvoAI::NodeGene
     */
    template<>
    struct hash<EvoAI::NodeGene>{
        using argument_type = EvoAI::NodeGene;
        using result_type = std::size_t;
        result_type operator()(const argument_type& ng) const{
            result_type seed = 0u;
            // better way of checking if x32 or x64 system?
            if constexpr(sizeof(std::size_t) == 8){
                assert(ng.getNeuronID() <= 1152921504606846975);//assert failed because Neuron is over 60 bits
                seed = (ng.getLayerID() << 62ll) | ng.getNeuronID();
            }else if constexpr(sizeof(std::size_t) == 4){
                assert(ng.getNeuronID() <= 1073741823); //assert failed because Neuron is over 30 bits
                seed = (ng.getLayerID() << 30ll) | ng.getNeuronID();
            }else{
                static_assert(sizeof(result_type) == 4 || sizeof(result_type) == 8, "NodeGene.hpp std::hash<NodeGene> : System must to be x86 or x64");
            }
            return seed;
        }
    };
}

#endif // EVOAI_NODE_GENE_HPP