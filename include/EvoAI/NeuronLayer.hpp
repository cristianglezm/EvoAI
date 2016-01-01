#ifndef NEURON_LAYER_HPP
#define NEURON_LAYER_HPP

#include <EvoAI/Neuron.hpp>
#include <EvoAI/Connection.hpp>
#include <vector>
#include <algorithm>
#include <memory>

namespace EvoAI{
    /**
     * @brief 
     */
    class NeuronLayer final{
        public:
            enum ActivationType{
                SIGMOID,
                TANH,
                SINUSOID,
                SOFTMAX
            };
        public:
            /**
             *
             */
            NeuronLayer();
            /**
             *
             */
            NeuronLayer(const std::size_t& numNeurons,const Neuron::Type& t,const double& bias);
            /**
             *
             */
            std::vector<Neuron>& getNeurons();
            /**
             *
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
            Neuron& operator[](const std::size_t& index);
            bool operator==(const NeuronLayer& rhs) const;
            ~NeuronLayer() = default;
        private:
            std::vector<Neuron> neurons;
            Neuron::Type type;
            ActivationType activationType;
            double bias;
    };
}

#endif // NEURON_LAYER_HPP
