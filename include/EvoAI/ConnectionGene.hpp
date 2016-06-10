#ifndef EVOAI_CONNECTION_GENE_HPP
#define EVOAI_CONNECTION_GENE_HPP

#include <JsonBox.h>

#include <EvoAI/NodeGene.hpp>
#include <EvoAI/Connection.hpp>
#include <EvoAI/Utils.hpp>

#include <string>

namespace EvoAI{
    /**
     * @class ConnectionGene
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief ConnectionGene for the NEAT algorithm
     */
    class ConnectionGene final{
        public:
            /**
             * @brief Constructor
             * @param src NodeGene
             * @param dest NodeGene
             * @param w double
             * @return ConnectionGene
             */
            ConnectionGene(const NodeGene& src, const NodeGene& dest, double w);
            /**
             * @brief Constructor
             * @param src Link
             * @param dest Link
             * @param w double weight
             * @param innv std::size_t innovationID
             * @return ConnectionGene
             */
            ConnectionGene(const Link& src, const Link& dest, double w);
            /**
             * @brief Constructor with json object
             * @param o JsonBox::Object&
             */
            ConnectionGene(JsonBox::Object o);
            /**
             * @brief returns if is enabled
             * @return bool
             */
            bool isEnabled() const noexcept;
            /**
             * @brief setter for enabled
             * @param en
             */
            void setEnabled(bool en) noexcept;
            /**
             * @brief returns a json of the connectionGene
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief setter for weight
             * @param w
             */
            void setWeight(double w) noexcept;
            /**
             * @brief getter for weight
             * @return double
             */
            const double& getWeight() const noexcept;
            /**
             * @brief getter for src
             * @return Link
             */
            const Link& getSrc() const noexcept;
            /**
             * @brief getter for dest
             * @return Link
             */
            const Link& getDest() const noexcept;
            /**
             * @brief getter for connection
             * @return Connection&
             */
            const Connection& getConnection() const noexcept;
            /**
             * @brief setter for frozen
             * set true to not change the weight
             * @param frzen 
             */
            void setFrozen(bool frzen) noexcept;
            /**
             * @brief getter for frozen
             * @return bool
             */
            bool isFrozen() const noexcept;
            /**
             * @brief setter for innovationID
             * @param id std::size_t
             */
            void setInnovationID(const std::size_t& id) noexcept;
            /**
             * @brief getter for innovationID
             * @return std::size_t
             */
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
namespace std{
    template<>
    struct hash<EvoAI::ConnectionGene>{
        using argument_type = EvoAI::ConnectionGene;
        using result_type = std::size_t;
        result_type operator()(argument_type const& cg) const{
            result_type const h1(std::hash<std::size_t>{}(cg.getSrc().layer));
            auto srcHash = EvoAI::hashCombine(h1,cg.getSrc().neuron);
            result_type const h2(std::hash<std::size_t>{}(cg.getDest().layer));
            auto destHash = EvoAI::hashCombine<std::size_t>(h2,cg.getDest().neuron);
            return EvoAI::hashCombine<std::size_t>(srcHash, destHash);
        }
    };
}
#endif // EVOAI_CONNECTION_GENE_HPP