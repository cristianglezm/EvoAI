#ifndef EVOAI_NEURON_LAYER_HPP
#define EVOAI_NEURON_LAYER_HPP

#include <vector>
#include <algorithm>
#include <memory>

#include <JsonBox.h>

#include <EvoAI/Neuron.hpp>
#include <EvoAI/Export.hpp>
#include <EvoAI/Connection.hpp>

namespace EvoAI{
    /**
     * @class NeuronLayer
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief A neuron layer.
     */
    class EvoAI_API NeuronLayer final{
        public:
            /**
             * @brief default contructor
             */
            NeuronLayer();
            /**
             * @brief Constructor that will build the neurons with random bias weights.
             * @param numNeurons std::size_t
             * @param t const Neuron::Type&
             * @param Bias double
             */
            NeuronLayer(std::size_t numNeurons, const Neuron::Type& t,double Bias);
            /**
             * @brief Constructor from json file
             * @param o JsonBox::Object
             */
            NeuronLayer(JsonBox::Object o);
            /**
             * @brief getter for the neurons
             * @return std::vector<Neuron>&
             */
            std::vector<Neuron>& getNeurons();
            /**
             * @brief takes a neuron vector and steals it.
             * @param ns std::vector<Neuron>&&
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
            inline const Neuron::Type& getType() const{ return type; }
            /**
             * @brief setter for bias
             * @param Bias double
             * @return NeuronLayer&
             */
            NeuronLayer& setBias(double Bias);
            /**
             * @brief getter for the bias
             * @return double
             */
            inline double getBias() const{ return bias; }
            /**
             * @brief adds a neuron
             * @param n const Neuron&
             * @return NeuronLayer&
             */
            NeuronLayer& addNeuron(const Neuron& n);
            /**
             * @brief Removes a Neuron from the layer
             * This method should be called when there are no connections.
             * @warning It does not remove connections if some other neuron has a connection 
             * to this neuron it won't be removed and it will cause a segmentation fault,
             * use NeuralNetwork::removeNeuron Instead.
             * @param n Neuron*
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
             * @warning Before adding a Connection you need to add the neurons.
             * @param c const Connection&
             * @return NeuronLayer&
             */
            NeuronLayer& addConnection(const Connection& c);
            /**
             * @brief removes a connection
             * @warning the connection could be invalid.
             * @param c const Connection&
             * @return bool
             */
            bool removeConnection(const Connection& c);
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
            inline Neuron::ActivationType getActivationType() const noexcept { return activationType; }
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
            NeuronLayer& setCyclesLimit(int cycles);
            /**
             * @brief getter for the cycles limit
             * @return int
             */
            inline int getCyclesLimit() const noexcept { return cyclesLimit; }
            /**
             * @brief returns a JsonBox::Value with the current info.
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const;
            /**
             * @brief Direct Access to Neurons
             * @warning Does not check if index is out of range
             * @code
             *      NeuronLayer[2].getOutput() // gets outputs of neuron 2
             * @endcode
             * @param index std::size_t
             * @return Neuron&
             */
            Neuron& operator[](std::size_t index);
            /**
             * @brief Direct Access to Neurons
             * @param index std::size_t
             * @return Neuron& const
             */
            const Neuron& operator[](std::size_t index) const;
            /**
             * @brief compare layers
             * @param rhs
             * @return bool
             */
            bool operator==(const NeuronLayer& rhs) const;
            /**
             * @brief it will return gradients from its neurons.
             * @code
             *      // ...
             *      nn2.backward(lossFn.backward(...));
             *      nn1.backward(nn2[0].backward());
             *      // ...
             * @endcode
             * @warning It should be used after calling NeuralNetwork::backward to connect input layer to another NeuralNetwork.
             * @return std::vector<double> gradients 
             */
            std::vector<double> backward() const noexcept;
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