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
         * @param layer const std::size_t&
         * @param neuron const std::size_t&
         * @return Link
         */
        Link(const std::size_t& layer, const std::size_t& neuron);
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
             * @return Connection
             */
            Connection(const Link& src, const Link& dest);
            /**
             * @brief Constructor with src, dest and weight
             * @param src Link&
             * @param dest Link&
             * @param w const double& Weight of the connection
             * @return Connection
             */
            Connection(const Link& src, const Link& dest, const double& w);
            /**
             * @brief Constructor json object
             * @param o JsonBox::Object&
             */
            Connection(JsonBox::Object o);
            /**
             * @brief check if is a recurrent connection
             * @return bool 
             */
            inline bool isRecurrent() const noexcept{ return !(src<dest); }
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
            inline const Link& getSrc() const noexcept{ return src; }
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
            inline const Link& getDest() const noexcept{ return dest; }
            /**
             * @brief checks if this connection has been visited.
             * @return bool
             */
            inline const bool& isVisited() const noexcept{ return visited; }
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
            inline const double& getWeight() const noexcept{ return weight; }
            /**
             * @brief setter for Weight
             * @param w Weight
             * @return Connection&
             */
            Connection& setWeight(const double& w);
            /**
             * @brief setter for cycles
             * @param c cycles
             * @return Connection&
             */
            Connection& setCycles(const int& c);
            /**
             * @brief getter for cycles
             * @return int&
             */
            inline const int& getCycles() const noexcept{ return cycles; }
            /**
             * @brief returns a string with the info of the connection.
             * @return std::string
             */
            std::string toString() const noexcept;
            /**
             * @brief passes the Connection to json
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
            Connection& setGradient(const double& grad) noexcept;
            /**
             * @brief adds the val to gradients
             * @param val
             * @return Connection
             */
            Connection& addGradient(const double& val) noexcept;
            /**
             * @brief getter for gradients
             * @return double&
             */
            inline const double& getGradient() const noexcept{ return gradient; }
            /**
             * @brief setter for delta
             * @param val
             * @return Connection&
             */
            Connection& setDelta(const double& val) noexcept;
            /**
             * @brief 
             * @return 
             */
            inline const double& getDelta() const noexcept{ return delta; }
            inline const double& getOldDelta() const noexcept{ return oldDelta; }
            /**
             * @brief resets the connection, visited, and gradient.
             */
            void reset();
        private:
            Link src;
            Link dest;
            double weight;
            int cycles;
            bool visited;
            double gradient;
            double delta;
            double oldDelta;
    };
}
#endif // EVOAI_CONNECTION_HPP