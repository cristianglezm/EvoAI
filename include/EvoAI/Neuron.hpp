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
            inline const double& getOutput() const{ return output; }
            /**
             * @brief 
             * @param out
             * @return 
             */
            Neuron& setOutput(const double& out);
            /**
             * @brief 
             * @param sum
             * @return 
             */
            Neuron& addSum(const double& sum);
            /**
             * @brief oldValue getter
             * @return double&
             */
            inline const double& getSum() const{ return sum; }
            /**
             * @brief 
             * @param sum
             * @return 
             */
            Neuron& setSum(const double& sum);
            /**
             * @brief error getter
             * @return double&
             */
            inline const double& getDelta() const{ return delta; }
            /**
             * @brief 
             * @param delta
             * @return 
             */
            Neuron& setDelta(const double& delta);
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
            JsonBox::Value toJson() const;
            /**
             * @brief 
             * @return 
             */
            std::string toString() const;
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
            /**
             * @brief 
             */
            void clearConnections();
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
