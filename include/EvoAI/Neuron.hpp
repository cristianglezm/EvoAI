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
            /**
             * @brief returns Neuron::Type to String
             * @param t
             * @return std::string
             */
            static std::string typeToString(Neuron::Type t) noexcept;
            /**
             * @brief returns a string Neuron::Type
             * @param t
             * @return int
             */
            static Type typeToEnum(const std::string& t) noexcept;
            /**
             * @brief Type of activation that will be used for the neuron.
             */
            enum ActivationType{
                IDENTITY,
                SIGMOID,
                STEPPED_SIGMOID,
                TANH,
                SINUSOID,
                RELU,
                NOISY_RELU,
                LEAKY_RELU,
                EXPONENTIAL,
                SOFTMAX,
                GAUSSIAN
            };
            /**
             * @brief returns Neuron::ActivationType to String
             * @param at
             * @return std::string
             */
            static std::string activationTypeToString(Neuron::ActivationType at) noexcept;
            /**
             * @brief returns a string Neuron::ActivationType
             * @param at
             * @return int
             */
            static ActivationType activationTypeToEnum(const std::string& at) noexcept;
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
            Neuron& setType(Type t) noexcept;
            /**
             * @brief getter for Neuron::Type
             * @return Type&
             */
            inline Type getType() const noexcept{ return type; }
            /**
             * @brief getter for output
             * @return double&
             */
            inline const double& getOutput() const noexcept{ return output; }
            /**
             * @brief setter for Output
             * @param out const double&
             * @return Neuron&
             */
            Neuron& setOutput(const double& out) noexcept;
            /**
             * @brief add value to Sum
             * @param sum const double&
             * @return Neuron&
             */
            Neuron& addSum(const double& val) noexcept;
            /**
             * @brief getter for sum
             * @return const double&
             */
            inline const double& getSum() const noexcept{ return sum; }
            /**
             * @brief setter for the sum
             * @param sum const double&
             * @return Neuron&
             */
            Neuron& setSum(const double& sum) noexcept;
            /**
             * @brief error getter
             * @return double&
             */
            inline const double& getDelta() const noexcept{ return delta; }
            /**
             * @brief Delta value for backprop
             * @param delta const double&
             * @return Neuron&
             */
            Neuron& setDelta(const double& delta) noexcept;
            /**
             * @brief resets the neuron if is not Type:CONTEXT
             * @return Neuron&
             */
            Neuron& reset() noexcept;
            /**
             * @brief resets the neuron if is Type:CONTEXT
             * @return Neuron&
             */
            Neuron& resetContext() noexcept;
            /**
             * @brief Returns a json value from the object
             * @return JsonBox::Value json 
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief Serialize the neuron info to string.
             * @return std::string
             */
            std::string toString() const noexcept;
            /**
             * @brief Adds a Connection.
             * @param c const Connection&
             * @return Neuron&
             */
            Neuron& addConnection(const Connection& c) noexcept;
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
            inline std::vector<Connection>& getConnections() noexcept{ return connections; }
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
            inline const double& getBiasWeight() const noexcept{ return biasWeight; }
            /**
             * @brief Setter for the bias Weight.
             * @param bw biasWeight
             * @return Neuron&
             */
            Neuron& setBiasWeight(const double& bw) noexcept;
            /**
             * @brief Clears the neuron's connections.
             */
            void clearConnections() noexcept;
            /**
             * @brief returns true if has some connection.
             * @return bool
             */
            bool hasConnections() const noexcept{ return (connections.size() > 0); }
            /**
             * @brief Returns the type of activation that will be used to trigger this neuron.
             * @return Neuron::ActivationType
             */
            ActivationType getActivationType() const noexcept{ return activationType; }
            /**
             * @brief setter for activation type
             * @param atype Neuron::ActivationType
             * @return Neuron&
             */
            Neuron& setActivationType(Neuron::ActivationType atype) noexcept;
            ~Neuron() = default;
        private:
            double output;
            double sum;
            double delta;
            double biasWeight;
            Type type;
            ActivationType activationType;
            std::vector<Connection> connections;
    };
}

#endif // NEURON_HPP
