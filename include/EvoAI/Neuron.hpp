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
     * @brief EvoAI::Neuron class
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     */
    class Neuron final{
        public:
            /**
             * @brief Neuron::Type defines the type of a neuron.
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
             * @brief Constructor that takes a type parameter.
             * @param const Type& t
             */
            Neuron(Type t);
            /**
             * @brief type setter
             */
            Neuron& setType(Type t);
            /**
             * @brief type getter
             * @return Type&
             */
            inline Type getType() const{ return type; }
            /**
             * @brief get value
             * @return double&
             */
            inline const double& getValue() const{ return value; }
            /**
             * @brief Value setter sets oldvalue to value and value to new value.
             * @return Neuron&
             */
            Neuron& setValue(const double& val);
            /**
             * @brief 
             * @param val
             * @return 
             */
            Neuron& addValue(const double& val);
            /**
             * @brief oldValue getter
             * @return double&
             */
            inline const double& getOldValue() const{ return oldValue; }
            /**
             * @brief error getter
             * @return double&
             */
            inline const double& getError() const{ return error; }
            /**
             * @brief error setter
             * @param double& err
             * @return Neuron&
             */
            Neuron& setError(const double& err);
            /**
             * @brief resets the neuron if is not Type:CONTEXT
             * @return Neuron&
             */
            Neuron& reset();
            /**
             * @brief resets the neuron Context
             * @return Neuron&
             */
            Neuron& resetContext();
            /**
             * @brief 
             * @return JsonBox::Value json 
             */
            JsonBox::Value toJson();
            /**
             * @brief 
             * @return 
             */
            std::string toString();
            /**
             * @brief 
             * @param c
             * @return 
             */
            Neuron& addConnection(const Connection& c);
            /**
             * @brief 
             * @param c
             * @return 
             */
            bool removeConnection(const Connection& c);
            /**
             * @brief 
             * @return 
             */
            inline std::vector<Connection>& getConnections(){ return connections; }
            /**
             * @brief 
             * @param rhs
             */
            bool operator==(const Neuron& rhs) const;
            /**
             * @brief Getter
             * @return const double& biasWeight
             */
            inline const double& getBiasWeight() const{ return biasWeight; }
            /**
             * @brief 
             * @param bw
             * @return 
             */
            Neuron& setBiasWeight(const double& bw);
            ~Neuron() = default;
        private:
            double value;
            double oldValue;
            double error;
            double biasWeight;
            Type type;
            std::vector<Connection> connections;
    };
}

#endif // NEURON_HPP
