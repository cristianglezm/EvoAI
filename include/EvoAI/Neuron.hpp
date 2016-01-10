#ifndef NEURON_HPP
#define NEURON_HPP

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <JsonBox.h>
#include <EvoAI/Connection.hpp>

namespace EvoAI{
    /**
     * @class Neuron
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief Defines a neuron
     */
    class Neuron final{
        public:
            /**
             * @brief Neuron::Type defines the type of a neuron.
             * CONTEXT: makes the neuron remember outputs from other neurons
             * HIDDEN: is a hidden neuron.
             * INPUT: this neuron will take the values provided and just pass them along.
             * OUTPUT: this neuron will hold the results.
             */
            enum class Type{
                CONTEXT,
                HIDDEN,
                INPUT,
                OUTPUT
            };
        public:
            /**
             * @brief Default Constructor type defaults to Type::HIDDEN
             */
            Neuron();
            /**
             * @brief Constructor that takes a Neuron::Type parameter.
             * @param t const Type&
             */
            Neuron(Type t);
            /**
             * @brief Setter for Neuron::Type
             * @param t Neuron::Type
             * @return Neuron&
             */
            Neuron& setType(Type t);
            /**
             * @brief getter for Neuron::Type
             * @return Type&
             */
            inline Type getType() const{ return type; }
            /**
             * @brief getter for output
             * @return double&
             */
            inline const double& getOutput() const{ return output; }
            /**
             * @brief setter for Output
             * @param out const double&
             * @return Neuron&
             */
            Neuron& setOutput(const double& out);
            /**
             * @brief add value to Sum
             * @param sum const double&
             * @return Neuron&
             */
            Neuron& addSum(const double& val);
            /**
             * @brief getter for sum
             * @return const double&
             */
            inline const double& getSum() const{ return sum; }
            /**
             * @brief setter for the sum
             * @param sum const double&
             * @return Neuron&
             */
            Neuron& setSum(const double& sum);
            /**
             * @brief error getter
             * @return double&
             */
            inline const double& getDelta() const{ return delta; }
            /**
             * @brief Delta value for backprop
             * @param delta const double&
             * @return Neuron&
             */
            Neuron& setDelta(const double& delta);
            /**
             * @brief resets the neuron if is not Type:CONTEXT
             * @return Neuron&
             */
            Neuron& reset();
            /**
             * @brief resets the neuron if is Type:CONTEXT
             * @return Neuron&
             */
            Neuron& resetContext();
            /**
             * @brief Returns a json value from the object
             * @return JsonBox::Value json 
             */
            JsonBox::Value toJson() const;
            /**
             * @brief Serialize the neuron info to string.
             * @return std::string
             */
            std::string toString() const;
            /**
             * @brief Adds a Connection.
             * @param c const Connection&
             * @return Neuron&
             */
            Neuron& addConnection(const Connection& c);
            /**
             * @brief Removes a Connection
             * @param const Connection& c
             * @return bool
             */
            bool removeConnection(const Connection& c);
            /**
             * @brief Getter for Connections
             * @return std::vector<Connection>&
             */
            inline std::vector<Connection>& getConnections(){ return connections; }
            /**
             * @brief Equality Operator
             * @param rhs
             * @return bool
             */
            bool operator==(const Neuron& rhs) const;
            /**
             * @brief Getter for the Bias Weight
             * @return const double& biasWeight
             */
            inline const double& getBiasWeight() const{ return biasWeight; }
            /**
             * @brief Setter for the bias Weight.
             * @param bw biasWeight
             * @return Neuron&
             */
            Neuron& setBiasWeight(const double& bw);
            /**
             * @brief Clears the neuron's connections.
             */
            void clearConnections();
            /**
             * @brief returns true if has some connection.
             * @return bool
             */
            bool hasConnections() const{ return (connections.size() > 0); }
            ~Neuron() = default;
        private:
            double output;
            double sum;
            double delta;
            double biasWeight;
            Type type;
            std::vector<Connection> connections;
    };
}

#endif // NEURON_HPP
