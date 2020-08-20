#ifndef EVOAI_CONNECTION_GENE_HPP
#define EVOAI_CONNECTION_GENE_HPP

#include <JsonBox.h>

#include <EvoAI/NodeGene.hpp>
#include <EvoAI/Connection.hpp>
#include <EvoAI/Utils.hpp>

#include <string>
#include <cassert>

namespace EvoAI{
    /**
     * @class ConnectionGene
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief ConnectionGene for the NEAT algorithm
     */
    class EvoAI_API ConnectionGene final{
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
             *  @brief copy constructor
             *  
             *  @param [in] rhs const ConnectionGene&
             *  @return bool
             */
            ConnectionGene(const ConnectionGene& rhs) noexcept;
            /**
             *  @brief move constructor
             *  
             *  @param [in] rhs ConnectionGene&&
             *  @return bool
             */
            ConnectionGene(ConnectionGene&& rhs) noexcept;
            void operator=(const ConnectionGene& rhs) noexcept;
            void operator=(ConnectionGene&& rhs) noexcept;
            constexpr bool operator<(const ConnectionGene& rhs) const noexcept;
            constexpr bool operator>(const ConnectionGene& rhs) const noexcept;
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
             * @brief returns a std::string
             * @return std::string
             * @todo remove and use operator<<
             */
            std::string toString(const std::string& delimiter = " ") const noexcept;
            /**
             * @brief returns a json of the connectionGene
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief adds the amount to the current weight.
             * @param amount
             */
            void addWeight(const double& amount) noexcept;
            /**
             * @brief setter for weight
             * @param w
             */
            void setWeight(const double& w) noexcept;
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
    constexpr bool ConnectionGene::operator<(const ConnectionGene& rhs) const noexcept{
        return innovationID < rhs.innovationID;
    }
    constexpr bool ConnectionGene::operator>(const ConnectionGene& rhs) const noexcept{
        return !((*this) < rhs);
    }
}
namespace std{
    /**
     * @brief specialization of std::hash for EvoAI::ConnectionGene
     */
    template<>
    struct hash<EvoAI::ConnectionGene>{
        using argument_type = EvoAI::ConnectionGene;
        using result_type = std::size_t;
        result_type operator()(const argument_type& cg) const{
            result_type seed = 0u;
            // better way of checking if x32 or x64 system?
            if constexpr(sizeof(result_type) == 8){
                assert(cg.getSrc().neuron <= 1073741823 || cg.getDest().neuron <= 1073741823);//assert Failed because Neuron is over 30 bits
                seed = ((cg.getSrc().layer << 62ll) | (cg.getSrc().neuron << 32ll)) | ((cg.getDest().layer << 30ll) | cg.getDest().neuron);
            }else if constexpr(sizeof(result_type) == 4){
                assert(cg.getSrc().neuron <= 16383 || cg.getDest().neuron <= 16383);//assert Failed because Neuron is over 14 bits
                seed = ((cg.getSrc().layer << 30ll) | (cg.getSrc().neuron << 16ll)) | ((cg.getDest().layer << 14ll) | cg.getDest().neuron);
            }else{
                static_assert(sizeof(result_type) == 4 || sizeof(result_type) == 8, "ConnectionGene.hpp std::hash<ConnectionGene> : System must be x86 or x64");
            }
            return seed;
        }
    };
}
#endif // EVOAI_CONNECTION_GENE_HPP