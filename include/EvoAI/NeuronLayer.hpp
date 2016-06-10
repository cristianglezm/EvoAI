#ifndef EVOAI_NEURON_LAYER_HPP
#define EVOAI_NEURON_LAYER_HPP

#include <vector>
#include <algorithm>
#include <memory>

#include <JsonBox.h>
#include <EvoAI/Neuron.hpp>
#include <EvoAI/Connection.hpp>

namespace EvoAI{
    /**
     * @class NeuronLayer
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief A neuron layer.
     */
    class NeuronLayer final{
        public:
            /**
             * @brief default contructor
             * @return NeuronLayer
             */
            NeuronLayer();
            /**
             * @brief Constructor that will build the neurons with random bias weights.
             * @param numNeurons
             * @param t
             * @param bias
             * @return NeuronLayer
             */
            NeuronLayer(const std::size_t& numNeurons,const Neuron::Type& t,const double& bias);
            /**
             * @brief Constructor from json file
             * @param o JsonBox::Object
             * @return NeuronLayer
             */
            NeuronLayer(JsonBox::Object o);
            /**
             * @brief getter for the neurons
             * @return std::vector<Neuron>&
             */
            std::vector<Neuron>& getNeurons();
            /**
             * @brief takes a neuron vector and steals it.
             * @param ns
             * @return NeuronLayer&
             */
            NeuronLayer& setNeurons(std::vector<Neuron>&& ns);
            /**
             * @brief gives the how many neurons it has.
             */
            std::size_t size() const;
            /**
             * @brief setter for Neuron::Type
             * will traverse each neuron and set the type to t
             * @param t Neuron::Type
             * @return NeuronLayer&
             */
            NeuronLayer& setType(Neuron::Type t);
            /**
             * @brief getter for Neuron::Type
             * @return Neuron::Type&
             */
            inline const Neuron::Type& getType() const { return type; }
            /**
             * @brief setter for bias
             * @param bias
             * @return NeuronLayer&
             */
            NeuronLayer& setBias(const double& bias);
            /**
             * @brief getter for the bias
             * @return const double&
             */
            inline const double& getBias() const{ return bias; }
            /**
             * @brief adds a neuron
             * @param n const Neuron&
             * @return NeuronLayer&
             */
            NeuronLayer& addNeuron(const Neuron& n);
            /**
             * @brief Removes a Neuron from the layer
             * This method should be called when there are no connections.
             * DOES NOT REMOVE CONNNECTIONS IF SOME NEURON HAS A CONNECTION
             * TO THIS NEURON IT WON'T BE REMOVED AND WILL CAUSE A SEGMENTATION FAULT.
             * USE removeNeuron Instead.
             * @param n
             * @return bool 
             */
            bool removeNeuron(Neuron* n);
            /**
             * @brief Checks if a neuron is from this layer
             * @param n Neuron*
             * @return bool
             */
            bool hasNeuron(Neuron* n);
            /**
             * @brief adds a Connection
             * Before adding a Connection you need to add the neurons.
             * @param c
             * @return NeuronLayer&
             */
            NeuronLayer& addConnection(const Connection& c);
            /**
             * @brief removes a connection
             * @param c
             * @return bool
             */
            bool removeConnection(Connection& c);
            /**
             * @brief will reset the layer's neurons.
             */
            void reset();
            /**
             * @brief will reset the layer's neurons that are Neuron::Type::CONTEXT
             */
            void resetContext();
            /**
             * @brief getter for the ActivationType
             * @return Neuron::ActivationType
             */
            Neuron::ActivationType getActivationType() const{ return activationType; }
            /**
             * @brief setter for activation type
             * It will traverse each neuron and set the ActivationType to atype.
             * @param atype Neuron::ActivationType
             * @return NeuronLayer&
             */
            NeuronLayer& setActivationType(Neuron::ActivationType atype);
            /**
             * @brief Sets how many cycles to reset the context neurons.
             * @param cycles
             * @return NeuronLayer&
             */
            NeuronLayer& setCyclesLimit(const int& cycles);
            /**
             * @brief getter for the cycles limit
             * @return const int&
             */
            inline const int& getCyclesLimit() const{ return cyclesLimit; }
            /**
             * @brief returns a JsonBox::Value with the current info.
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const;
            /**
             * @brief Direct Access to Neurons
             * Example:
             *      NeuronLayer[2].getOutput() // gets outputs of neuron 2
             * Does not check if index is out of range
             * @param index const std::size_t&
             * @return Neuron&
             */
            Neuron& operator[](const std::size_t& index);
            /**
             * @brief Direct Access to Neurons
             * @param index const std::size_t&
             * @return Neuron& const
             */
            const Neuron& operator[](const std::size_t& index) const;
            /**
             * @brief compare layers
             * @param rhs
             */
            bool operator==(const NeuronLayer& rhs) const;
            ~NeuronLayer() = default;
        private:
            std::vector<Neuron> neurons;
            Neuron::Type type;
            Neuron::ActivationType activationType;
            double bias;
            int cyclesLimit;
    };
}

#endif // EVOAI_NEURON_LAYER_HPP