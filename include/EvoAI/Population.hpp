#ifndef EVOAI_POPULATION_HPP
#define EVOAI_POPULATION_HPP

#include <vector>
#include <memory>
#include <utility>
#include <random>
#include <string>

#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Species.hpp>
#include <EvoAI/Genome.hpp>
#include <JsonBox.h>

namespace EvoAI{
    /**
     * @todo implement
     */
    class Population{
        public:
            Population();
            Population(const std::size_t& numInputs, const std::size_t& numOutputs, const std::size_t& size, bool canBeRecurrent = false, bool cppn = false);
            Population(JsonBox::Object o);
            Population(const std::string& filename);
            Genome* findGenome(const std::size_t& id) noexcept;
            Species* findSpecies(const std::size_t& id) noexcept;
            std::vector<std::unique_ptr<Genome>>& getGenomes() noexcept;
            std::vector<std::unique_ptr<Species>>& getSpecies() noexcept;
            void speciate(const double& CompatibilityThreshold = 6.0) noexcept;
            void reproduce(bool interSpecies) noexcept;
            void orderGenomesByFitness() noexcept;
            void orderSpeciesByAge() noexcept;
            void orderSpeciesByFitness() noexcept;
            void rankWithinSpecies() noexcept;
            void removeGenomesFromSpecies(const std::size_t& speciesID) noexcept;
            std::size_t getGenomesSize() const noexcept;
            std::size_t getSpeciesSize() const noexcept;
            JsonBox::Value toJson() const noexcept;
            void writeToFile(const std::string& filename);
            ~Population() = default;
        private:
            std::vector<std::unique_ptr<Genome>> genomes;
            std::vector<std::unique_ptr<Species>> species;
    };
}

#endif // EVOAI_POPULATION_HPP