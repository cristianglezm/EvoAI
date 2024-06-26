#ifndef EVOAI_NEURON_HPP
#define EVOAI_NEURON_HPP

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <JsonBox.h>
#include <EvoAI/Export.hpp>
#include <EvoAI/Connection.hpp>

namespace EvoAI{
    /**
     * @class Neuron
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief A basic neuron
     */
    class EvoAI_API Neuron final{
        public:
            /**
             * @brief Neuron::Type defines the type of a neuron.
             * CONTEXT: makes the neuron remember outputs from other neurons
             * HIDDEN: is a hidden neuron.
             * INPUT: this neuron will take the values provided and just pass them along.
             * OUTPUT: this neuron will hold the results.
             */
            enum Type{
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
                SINUSOID,
                COSINE,
                GAUSSIAN,
                SIGMOID,
                IDENTITY,
                MODULUS,
                STEPPED_SIGMOID,
                SWISH,
                TANH,
                TAN,
                RELU,
                NOISY_RELU,
                LEAKY_RELU,
                EXPONENTIAL,
                SQUARE,
                CUBE,
                SOFTPLUS,
                CLAMP,
                INV,
                LOG,
                ABS,
                HAT,
                LAST_CPPN_ACTIVATION_TYPE,
                SOFTMAX
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
             * @brief Default Constructor
             * type defaults to Type::HIDDEN
             * activationType defaults to ActivationType:
             */
            Neuron();
            /**
             * @brief Constructor for json object.
             * @param o JsonBox::Object
             */
            Neuron(JsonBox::Object o);
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
            inline double getOutput() const noexcept{ return output; }
            /**
             * @brief setter for Output
             * @param out double
             * @return Neuron&
             */
            Neuron& setOutput(double out) noexcept;
            /**
             * @brief add value to Sum
             * @param val double
             * @return Neuron&
             */
            Neuron& addSum(double val) noexcept;
            /**
             * @brief getter for sum
             * @return double
             */
            inline double getSum() const noexcept{ return sum; }
            /**
             * @brief setter for the sum
             * @param val double
             * @return Neuron&
             */
            Neuron& setSum(double val) noexcept;
            /**
             * @brief getter for gradient
             * @return double&
             */
            inline double getGradient() const noexcept{ return gradient; }
            /**
             * @brief gradient for backprop
             * @param gradient double
             * @return Neuron&
             */
            Neuron& setGradient(double gradient) noexcept;
            /**
             * @brief add to gradient
             * @param grad
             * @return Neuron&
             */
            Neuron& addGradient(double grad) noexcept;
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
             * @param delimiter the thing thats between the values.
             * example: "\n", "\t" etc.
             * @return std::string
             */
            std::string toString(const std::string& delimiter = ",") const noexcept;
            /**
             * @brief Adds a Connection.
             * @param c const Connection&
             * @return Neuron&
             */
            Neuron& addConnection(const Connection& c) noexcept;
            /**
             * @brief Removes a Connection
             * @param c const Connection&
             * @return bool
             */
            bool removeConnection(const Connection& c);
            /**
             * @brief Getter for Connections
             * @return std::vector<Connection>&
             */
            inline std::vector<Connection>& getConnections() noexcept{ return connections; }
            /**
             * @brief returns how many connections it has.
             * @return std::size_t
             */
            std::size_t size();
            /**
             * @brief direct access to connections.
             * @param index const std::size_t index
             * @return Connection&
             */
            Connection& operator[](std::size_t index);
            /**
             * @brief Equality Operator
             * @param rhs
             * @return bool
             */
            bool operator==(const Neuron& rhs) const;
            /**
             * @brief get a ptr to the internal connection bias(for parameters)
             * @return Connection*
             */
            inline Connection* getBiasPtr() noexcept{ return &biasWeight; }
            /**
             * @brief Getter for the Bias Weight
             * @return double biasWeight
             */
            inline double getBiasWeight() const noexcept{ return biasWeight.getWeight(); }
            /**
             * @brief Setter for the bias Weight.
             * @param bw biasWeight
             * @return Neuron&
             */
            Neuron& setBiasWeight(double bw) noexcept;
            /**
             * @brief setter for bias gradient
             * @param grad
             * @return double
             */
            Neuron& setBiasGradient(double grad) noexcept;
            /**
             * @brief getter for bias gradient
             * @return double
             */
            inline double getBiasGradient() const noexcept{ return biasWeight.getGradient(); }
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
            double gradient;
            Connection biasWeight;
            Type type;
            ActivationType activationType;
            std::vector<Connection> connections;
    };
}

#endif // EVOAI_NEURON_HPP
