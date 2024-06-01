#ifndef EVOAI_CONNECTION_HPP
#define EVOAI_CONNECTION_HPP

#include <string>
#include <sstream>
#include <JsonBox.h>

#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @class Link
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief A simple link defining a source point layer, neuron
     */
    struct EvoAI_API Link{
        /**
         * @brief default constructor
         * @param lyr std::size_t
         * @param nrn std::size_t
         */
        Link(std::size_t lyr, std::size_t nrn);
        Link(JsonBox::Object o);
        JsonBox::Value toJson() const noexcept;
        bool operator<(const Link& rhs) const;
        bool operator==(const Link& rhs) const;
        std::size_t layer;
        std::size_t neuron;
    };
    /**
     * @class Connection
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief A Connection used to connect neurons from different layers.
     */
    class EvoAI_API Connection{
        public:
            /**
             * @brief default Constructor
             */
            Connection();
            /**
             * @brief Constructor with src and dest
             * @param src Link&
             * @param dest Link&
             */
            Connection(const Link& src, const Link& dest);
            /**
             * @brief Constructor with src, dest and weight
             * @param src Link&
             * @param dest Link&
             * @param w double Weight of the connection
             */
            Connection(const Link& src, const Link& dest, double w);
            /**
             * @brief Constructor json object
             * @param o JsonBox::Object&
             */
            Connection(JsonBox::Object o);
            /**
             * @brief check if is a recurrent connection
             * @return bool 
             */
            inline bool isRecurrent() const noexcept;
            /**
             * @brief setter for src
             * @param l Link&
             * @return Connection&
             */
            Connection& setSrc(const Link& l);
            /**
             * @brief getter for src
             * @return Link& const
             */
            inline const Link& getSrc() const noexcept;
            /**
             * @brief setter for dest
             * @param l Link&
             * @return Connection&
             */
            Connection& setDest(const Link& l);
            /**
             * @brief getter for dest
             * @return Link& const
             */
            inline const Link& getDest() const noexcept;
            /**
             * @brief checks if this connection has been visited.
             * @return bool
             */
            inline const bool& isVisited() const noexcept;
            /**
             * @brief setter for visited
             * @param b 
             * @return Connection&
             */
            Connection& setVisited(bool b);
            /**
             * @brief getter for weight
             * @return double&
             */
            inline double getWeight() const noexcept;
            /**
             * @brief setter for Weight
             * @param w Weight
             * @return Connection&
             */
            Connection& setWeight(double w);
            /**
             * @brief setter for cycles
             * @param c cycles
             * @return Connection&
             */
            Connection& setCycles(int c);
            /**
             * @brief getter for cycles
             * @return int&
             */
            inline int getCycles() const noexcept;
            /**
             * @brief returns a string with the info of the connection.
             * @return std::string
             */
            std::string toString() const noexcept;
            /**
             * @brief converts the Connection to JsonBox::Value
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief compare connections
             * @param rhs
             */
            bool operator==(const Connection& rhs) const;
            /**
             * @brief setter for gradient
             * @param grad
             * @return Connection
             */
            Connection& setGradient(double grad) noexcept;
            /**
             * @brief adds the val to gradients
             * @param val
             * @return Connection
             */
            Connection& addGradient(double val) noexcept;
            /**
             * @brief getter for gradients
             * @return double&
             */
            inline double getGradient() const noexcept;
            /**
             * @brief resets the connection, visited, and gradient.
             */
            void reset();
            /**
             * @brief setter for frozen, set true to not change the weight
             * @param frzen bool
             * @return Connection&
             */
            Connection& setFrozen(bool frzen) noexcept;
            /**
             * @brief getter for frozen
             * @return bool
             */
            inline bool isFrozen() const noexcept;
        private:
            Link src;
            Link dest;
            double weight;
            double gradient;
            int cycles;
            bool visited;
            bool frozen;
    };
    bool Connection::isRecurrent() const noexcept{
        return (dest<src);
    }
    const Link& Connection::getSrc() const noexcept{
        return src;
    }
    const Link& Connection::getDest() const noexcept{
        return dest;
    }
    const bool& Connection::isVisited() const noexcept{
        return visited;
    }
    double Connection::getWeight() const noexcept{
        return weight;
    }
    int Connection::getCycles() const noexcept{
        return cycles;
    }
    double Connection::getGradient() const noexcept{
        return gradient;
    }
    bool Connection::isFrozen() const noexcept{
        return frozen;
    }
}
#endif // EVOAI_CONNECTION_HPP