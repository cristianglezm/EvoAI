#ifndef EVOAI_CONNECTION_GENE_HPP
#define EVOAI_CONNECTION_GENE_HPP

#include <JsonBox.h>

#include <EvoAI/NodeGene.hpp>
#include <EvoAI/Connection.hpp>

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
            JsonBox::Value toJson() noexcept;
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

#endif // EVOAI_CONNECTION_GENE_HPP