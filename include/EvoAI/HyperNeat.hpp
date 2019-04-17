#ifndef EVOAI_HYPERNEAT_HPP
#define EVOAI_HYPERNEAT_HPP

#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Genome.hpp>
#include <EvoAI/Export.hpp>

#include <vector>

namespace EvoAI{
    /**
     * @class SubstrateInfo
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief A simple struct describing the number of inputs, hidden layers and neurons and the number of outputs for the substrate.
     */
    struct EvoAI_API SubstrateInfo{
        /**
         * @brief default Constructor
         * @return SubstrateInfo
         */
        SubstrateInfo();
        /**
         * @brief Constructor with all the info.
         * @param numInputs const std::size_t&
         * @param numHiddenLayers const std::size_t&
         * @param numHiddenNeurons const std::size_t&
         * @param numOutputs const std::size_t&
         * @return SubstrateInfo
         */
        SubstrateInfo(const std::size_t& numInputs, const std::size_t& numHiddenLayers,
                        const std::size_t& numHiddenNeurons, const std::size_t& numOutputs);
        /**
         * @brief Constructor to load a JsonBox::Object
         * @param o JsonBox::Object
         * @return SubstrateInfo
         */
        SubstrateInfo(JsonBox::Object o);
        /**
         * @brief returns a json value
         * @return JsonBox::Value
         */
        JsonBox::Value toJson() const noexcept;
        ~SubstrateInfo() = default;
        std::size_t numInputs;
        std::size_t numHiddenLayers;
        std::size_t numHiddenNeurons;
        std::size_t numOutputs;
    };
    /**
     * @class HyperNeat
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief A HyperNeat is a Neural Network thats generates from a cppn.
     */
    class EvoAI_API HyperNeat{
    public:
            /**
             * @brief How the substrate will be decided.
             * GRID: requires a genome that has 3 inputs and 2 outputs.
             * SANDWICH: Requires a genome that has 5 inputs and 2 outputs.
             */
            enum SubstrateConfiguration{
                GRID,
                SANDWICH
            };
            /**
             * @brief enum to string
             * @param sc HyperNeat::SubstrateConfiguration
             * @return std::string
             */
            static std::string SubstrateConfigurationToString(SubstrateConfiguration sc) noexcept;
            /**
             * @brief string to enum
             * @param sc HyperNeat::SubstrateConfiguration
             * @return HyperNeat::SubstrateConfiguration
             */
            static SubstrateConfiguration SubstrateConfigurationToEnum(const std::string& sc) noexcept;
        public:
            /**
             * @brief constructor for a JsonBox::Object
             * @param o JsonBox::Object
             * @return HyperNeat
             */
            HyperNeat(JsonBox::Object o);
            /**
             * @brief constructor that loads a file wrote with HyperNeat::writeToFile
             * @param filename const std::string&
             * @return HyperNeat
             */
            HyperNeat(const std::string& filename);
            /**
             * @brief constructs a HyperNeat from the genome specicied, It copies the genome.
             * @param si const SubstrateInfo information about the substrate
             * @param g Genome& the genome
             * @param sc SubstrateConfiguration
             * @return HyperNeat
             */
            HyperNeat(const SubstrateInfo& si, Genome& g, SubstrateConfiguration sc);
            /**
             * @brief constructs a HyperNeat, makes the genome.
             * @param si const SubstrateInfo information about the substrate
             * @param sc SubstrateConfiguration
             * @return HyperNeat
             */
            HyperNeat(const SubstrateInfo& si, SubstrateConfiguration sc = SubstrateConfiguration::GRID);
            /**
             * @brief setter for numInputs
             * @param num const std::size_t&
             */
            void setNumInputs(const std::size_t& num) noexcept;
            /**
             * @brief getter for the numInputs
             * @return const std::size_t&
             */
            const std::size_t& getNumInputs() const noexcept;
            /**
             * @brief setter for numHiddenLayers
             * @param num const std::size_t&
             */
            void setNumHiddenLayers(const std::size_t& num) noexcept;
            /**
             * @brief getter for the numHiddenLayers
             * @return const std::size_t&
             */
            const std::size_t& getNumHiddenLayers() const noexcept;
            /**
             * @brief setter for numHiddenNeurons
             * @param num const std::size_t&
             */
            void setNumHiddenNeurons(const std::size_t& num) noexcept;
            /**
             * @brief getter for the numHiddenNeurons
             * @return const std::size_t&
             */
            const std::size_t& getNumHiddenNeurons() const noexcept;
            /**
             * @brief setter for numOutputs
             * @param num const std::size_t&
             */
            void setNumOutputs(const std::size_t& num) noexcept;
            /**
             * @brief getter for the numOutputs.
             * @return const std::size_t&
             */
            const std::size_t& getNumOutputs() const noexcept;
            /**
             * @brief setter for the genome.
             * @param g Genome&
             */
            void setGenome(Genome& g);
            /**
             * @brief getter for the genome.
             * @return const Genome&
             */
            const Genome& getGenome() const noexcept;
            /**
             * @brief getter for the substrate
             * @return NeuralNetwork&
             */
            NeuralNetwork& getSubstrate() noexcept;
            /**
             * @brief makes the substrate if is not in invalid state.
             * call this after the setters.
             */
            void makeSubstrate() noexcept;
            /**
             * @brief sets the inputs for the substrate.
             * @param inputs std::vector<double>&&
             */
            void setInputs(std::vector<double>&& inputs);
            /**
             * @brief runs the substrate
             * @return std::vector<double>
             */
            std::vector<double> run() noexcept;
            /**
             * @brief resets the calculations done in substrate
             */
            void reset() noexcept;
            /**
             * @brief mutates the genome
             * Rates 0.0-1.0
             * @param nodeRate float
             * @param connectionRate float
             * @param perturbWeightsRate float
             * @param enableRate float
             * @param disableRate float
             * @param actTypeRate float
             */
            void mutate(float nodeRate = 0.2, float addConnRate = 0.3, float removeConnRate = 0.2,
                                        float perturbWeightsRate = 0.6, float enableRate = 0.35,
                                        float disableRate = 0.3, float actTypeRate = 0.4) noexcept;
            /**
             * @brief setter for fitness
             * @param fitness const double&
             */
            void setFitness(const double& fitness) noexcept;
            /**
             * @brief getter for fitness
             * @return const double&
             */
            const double& getFitness() const noexcept;
            /**
             * @brief changes the activation type for an entire layer in the substrate.
             * @param at Neuron::ActivationType
             */
            void setActivationType(const std::size_t& layer, Neuron::ActivationType at) noexcept;
            /**
             * @brief returns a JsonBox::Value thats the HyperNeat.
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief writes the HyperNeat object to a file.
             * @param filename const std::string&
             */
            void writeToFile(const std::string& filename) const noexcept;
            ~HyperNeat() = default;
        private:
            Genome genome;
            NeuralNetwork substrate;
            SubstrateInfo substrateInfo;
            SubstrateConfiguration substrateConf;
            bool isSubstrateValid;
            bool isSubstrateReady;
    };
}

#endif // EVOAI_HYPERNEAT_HPP