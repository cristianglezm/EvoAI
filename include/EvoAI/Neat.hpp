#ifndef EVOAI_NEAT_HPP
#define EVOAI_NEAT_HPP

#include <vector>
#include <memory>
#include <utility>
#include <random>
#include <string>

#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Genome.hpp>
#include <JsonBox.h>

namespace EvoAI{
    /**
     * @class Neat
     * @author Cristian Glez <cristian.glez.m@gmail.com>
     * @brief Neat Algorithm
     */
    class Neat final{
        public:
            Neat(const std::size_t& numInputs, const std::size_t& numOutputs);
            Neat(const std::string& filename);
            JsonBox::Value toJson() const noexcept;
            NeuralNetwork* getPhenotype() noexcept;
            bool writeToFile(const std::string& filename) const;
            ~Neat() = default;
        private:
            std::unique_ptr<Genome> genome;
            std::unique_ptr<NeuralNetwork> phenotype;
    };
}
#endif // EVOAI_NEAT_HPP