#ifndef EVOAI_HYPERNEAT_HPP
#define EVOAI_HYPERNEAT_HPP

#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Genome.hpp>
#include <EvoAI/Export.hpp>

#include <vector>

namespace EvoAI{
    /**
     * @class SubstrateInfo
     * @author Cristian
     * @brief 
     * @todo
     */
    struct EvoAI_API SubstrateInfo{
        SubstrateInfo();
        SubstrateInfo(const std::size_t& numInputs, const std::size_t& numHiddenLayers,
                        const std::size_t& numHiddenNeurons, const std::size_t& numOutputs);
        SubstrateInfo(JsonBox::Object o);
        JsonBox::Value toJson() const noexcept;
        ~SubstrateInfo() = default;
        std::size_t numInputs;
        std::size_t numHiddenLayers;
        std::size_t numHiddenNeurons;
        std::size_t numOutputs;
    };
    /**
     * @class HyperNeat
     * @author Cristian
     * @brief 
     * @todo
     */
    class EvoAI_API HyperNeat{
    public:
            /**
             * @brief
             */
            enum SubstrateConfiguration{
                GRID,
                SANDWICH
            };
            static std::string SubstrateConfigurationToString(SubstrateConfiguration sc) noexcept;
            static SubstrateConfiguration SubstrateConfigurationToEnum(const std::string& sc) noexcept;
        public:
            HyperNeat(JsonBox::Object o);
            HyperNeat(const std::string& filename);
            HyperNeat(const SubstrateInfo& si, Genome& g, SubstrateConfiguration sc);
            HyperNeat(const SubstrateInfo& si, SubstrateConfiguration sc = SubstrateConfiguration::GRID);
            void setNumInputs(const std::size_t& num) noexcept;
            const std::size_t& getNumInputs() const noexcept;
            void setNumHiddenLayers(const std::size_t& num) noexcept;
            const std::size_t& getNumHiddenLayers() const noexcept;
            void setNumHiddenNeurons(const std::size_t& num) noexcept;
            const std::size_t& getNumHiddenNeurons() const noexcept;
            void setNumOutputs(const std::size_t& num) noexcept;
            const std::size_t& getNumOutputs() const noexcept;
            void setGenome(Genome& g);
            const Genome& getGenome() const noexcept;
            NeuralNetwork& getSubstrate() noexcept;
            void makeSubstrate() noexcept;
            void setInputs(std::vector<double>&& inputs);
            std::vector<double> run() noexcept;
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
            void setFitness(const double& fitness) noexcept;
            const double& getFitness() const noexcept;
            JsonBox::Value toJson() const noexcept;
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