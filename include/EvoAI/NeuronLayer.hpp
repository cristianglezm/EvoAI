#ifndef NEURON_LAYER_HPP
#define NEURON_LAYER_HPP

#include <EvoAI/Neuron.hpp>
#include <EvoAI/Connection.hpp>
#include <vector>
#include <algorithm>
#include <memory>
#include <JsonBox.h>

namespace EvoAI{
    /**
     * @class NeuronLayer
     * @author Cristian
     * @brief 
     */
    class NeuronLayer final{
        public:
            enum ActivationType{
                IDENTITY,
                SIGMOID,
                STEPPED_SIGMOID,
                TANH,
                SINUSOID,
                RELU,
                NOISY_RELU,
                LEAKY_RELU,
                EXPONENTIAL
            };
        public:
            /**
             * @brief 
             * @return 
             */
            NeuronLayer();
            /**
             * @brief 
             * @return 
             */
            NeuronLayer(const std::size_t& numNeurons,const Neuron::Type& t,const double& bias);
            /**
             * @brief 
             * @return 
             */
            std::vector<Neuron>& getNeurons();
            /**
             * @brief 
             * @return 
             */
            NeuronLayer& setNeurons(std::vector<Neuron>&& ns);
            /**
             *
             */
            std::size_t size();
            /**
             *
             */
            NeuronLayer& setType(Neuron::Type& t);
            /**
             *
             */
            inline const Neuron::Type& getType() const { return type; }
            /**
             *
             */
            NeuronLayer& setBias(const double& bias);
            /**
             *
             */
            inline const double& getBias() const{ return bias; }
            /**
             *
             */
            NeuronLayer& addNeuron(const Neuron& n);
            /**
             *
             */
            bool removeNeuron(Neuron* n);
            /**
             *
             */
            NeuronLayer& addConnection(const Connection& c);
            /**
             *
             */
            bool removeConnection(Connection& c);
            /**
             *
             */
            void reset();
            /**
             *
             */
            void resetContext();
            /**
             * @brief 
             * @return 
             */
            ActivationType getActivationType() const{ return activationType; }
            /**
             * @brief 
             * @param atype
             * @return 
             */
            NeuronLayer& setActivationType(ActivationType atype);
            /**
             * @brief Sets how many cycles the layer context neurons will remember.
             * @param cycles&&
             * @return NeuronLayer&
             * TODO
             */
            NeuronLayer& setCyclesLimit(int&& cycles);
            /**
             * @brief 
             * @return 
             */
            inline const int& getCyclesLimit() const{ return cyclesLimit; }
            /**
             * @brief 
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const;
            Neuron& operator[](const std::size_t& index);
            bool operator==(const NeuronLayer& rhs) const;
            ~NeuronLayer() = default;
        private:
            std::vector<Neuron> neurons;
            Neuron::Type type;
            ActivationType activationType;
            double bias;
            int cyclesLimit;
    };
}

#endif // NEURON_LAYER_HPP
