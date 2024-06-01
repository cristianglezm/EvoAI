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
    struct EvoAI_API SubstrateInfo final{
        /**
         * @brief default Constructor
         */
        SubstrateInfo();
        /**
         * @brief Constructor with all the info.
         * @param nInputs std::size_t number of inputs
         * @param nHiddenLayers std::size_t number of hidden layers
         * @param nHiddenNeurons const std::vector<std::size_t>& numbers of hidden neurons in each hidden layer
         * @param nOutputs std::size_t number of outputs
         * @param LEO double leo min to set connection or not
         * @param minMaxWeight double min and max weight
         * @param Bias double bias for the network
         */
        SubstrateInfo(std::size_t nInputs, std::size_t nHiddenLayers, 
					const std::vector<std::size_t>& nHiddenNeurons, std::size_t nOutputs, double LEO = 0.5, double minMaxWeight = 8.0, double Bias = 1.0);
        /**
         * @brief Constructor to load a JsonBox::Object
         * @param o JsonBox::Object
         */
        SubstrateInfo(JsonBox::Object o);
        /**
         * @brief returns a JsonBox::Value
         * @return JsonBox::Value
         */
        JsonBox::Value toJson() const noexcept;
        ~SubstrateInfo() = default;
        std::size_t numInputs;
        std::size_t numHiddenLayers;
        std::vector<std::size_t> numHiddenNeurons;
        std::size_t numOutputs;
        double leo;
        double minmaxWeight;
        double bias;
    };
    class Genome;
    /**
     * @class HyperNeat
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief A HyperNeat is a Neural Network thats generates from a cppn.
     */
    class EvoAI_API HyperNeat final{
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
             */
            HyperNeat(JsonBox::Object o);
            /**
             * @brief constructor that loads a file wrote with HyperNeat::writeToFile
             * @param filename const std::string&
             */
            HyperNeat(const std::string& filename);
            /**
             * @brief constructs a HyperNeat from the genome specicied, It copies the genome.
             * @param si const SubstrateInfo information about the substrate
             * @param g Genome& the genome
             * @param sc SubstrateConfiguration
             */
            HyperNeat(const SubstrateInfo& si, Genome& g, SubstrateConfiguration sc);
            /**
             * @brief constructs a HyperNeat, makes the genome.
             * @param si const SubstrateInfo information about the substrate
             * @param sc SubstrateConfiguration
             */
            HyperNeat(const SubstrateInfo& si, SubstrateConfiguration sc = SubstrateConfiguration::GRID);
            /**
             * @brief setter for numInputs
             * @param num std::size_t
             */
            void setNumInputs(std::size_t num) noexcept;
            /**
             * @brief getter for the numInputs
             * @return std::size_t
             */
            std::size_t getNumInputs() const noexcept;
            /**
             * @brief setter for numHiddenLayers
             * @param num std::size_t
             */
            void setNumHiddenLayers(std::size_t num) noexcept;
            /**
             * @brief getter for the numHiddenLayers
             * @return std::size_t
             */
            std::size_t getNumHiddenLayers() const noexcept;
            /**
             * @brief setter for numHiddenNeurons
             * @param num const std::vector<std::size_t>&
             */
            void setNumHiddenNeurons(const std::vector<std::size_t>& num) noexcept;
            /**
             * @brief getter for the numHiddenNeurons
             * @return const std::vector<std::size_t>&
             */
            const std::vector<std::size_t>& getNumHiddenNeurons() const noexcept;
            /**
             * @brief setter for numOutputs
             * @param num std::size_t
             */
            void setNumOutputs(std::size_t num) noexcept;
            /**
             * @brief getter for the numOutputs.
             * @return std::size_t
             */
            std::size_t getNumOutputs() const noexcept;
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
             * @brief This assumes that subst is a valid substrate for this HyperNeat.
             * @details this can be used when the substrate has been trained using backward 
             *          but is recommended to use it directly.
             * @warning substrate will be made again if changed by calling a invalidating method.
             * @param subst NeuralNetwork&&
             */
            void setSubstrate(NeuralNetwork&& subst) noexcept;
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
             * @brief sets the inputs returns true if succeeded, false if it failed.
             * @param ins const std::vector<double>&
             */
            void setInputs(const std::vector<double>& ins) noexcept;
            /**
             * @brief runs the substrate
             * @return std::vector<double>
             */
            std::vector<double> run() noexcept;
            /**
             * @brief calls setsInput and calls run
             * @param input const std::vector<double>&
             * @return const std::vector<double>&
             */
            std::vector<double> forward(const std::vector<double>& input) noexcept;
            /**
             * @brief calls setsInput and calls run
             * @param input std::vector<double>&&
             * @return std::vector<double>&&
             */
            std::vector<double> forward(std::vector<double>&& input) noexcept;
            /**
             * @brief calculates the gradients for the network.
             * @warning It needs to be called before calling reset as the outputs and gradients would be 0.0
             * @code
             *     // ...
             *     auto outputs = nn.forward({...});
             *     nn.backward(lossFn{}.backward(...));
             *     optim.step(epoch);
             *     nn.reset() // it will reset the network
             * @endcode
             * @param gradientLoss std::vector<double>&&
             */
            void backward(std::vector<double>&& gradientLoss) noexcept;
            /**
             * @brief resets the calculations done in substrate
             */
            void reset() noexcept;
            /**
             * @brief mutates the genome, rates from 0.0 to 1.0
             * @param nodeRate float
             * @param addConnRate float
             * @param removeConnRate float
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
             * @param fitness double
             */
            void setFitness(double fitness) noexcept;
            /**
             * @brief getter for fitness
             * @return double
             */
            double getFitness() const noexcept;
            /**
             * @brief changes the activation type for an entire layer in the substrate.
             * @param layer std::size_t
             * @param at Neuron::ActivationType
             */
            void setActivationType(std::size_t layer, Neuron::ActivationType at) noexcept;
            /**
             * @brief returns a JsonBox::Value that's the HyperNeat.
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief writes the HyperNeat object to a file.
             * @param filename const std::string&
             */
            void writeToFile(const std::string& filename) const noexcept;
            /**
             * @brief writes a dot file for graphviz dot program.
             * @param filename
             * @return bool
             */
            bool writeDotFile(const std::string& filename) noexcept;
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
