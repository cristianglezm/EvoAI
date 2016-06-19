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
     * @todo refactor
     * species owns genomes
     * pop spawn and add to species
     * reproduce clears killable species
     *      interspecie rep triggers a spawn and add to species
     *      normal reproduction is % of the best species
     */
    class Population{
        public:
            Population();
            Population(const std::size_t& size, const std::size_t& numInputs, const std::size_t& numOutputs, bool canBeRecurrent = false, bool cppn = false);
            Population(JsonBox::Object o);
            Population(const std::string& filename);
            Genome* findGenome(const std::size_t& id) noexcept;
            Genome* getBestGenome() const noexcept;
            Species* findSpecies(const std::size_t& id) noexcept;
            std::vector<std::unique_ptr<Genome>>& getGenomes() noexcept;
            std::vector<std::unique_ptr<Species>>& getSpecies() noexcept;
            void speciate(const double& CompatibilityThreshold = 6.0, const std::size_t& maxAge = 20) noexcept;
            void reproduce(bool interSpecies) noexcept;
            void orderGenomesByFitness() noexcept;
            void orderSpeciesByAge() noexcept;
            void orderSpeciesByFitness() noexcept;
            void rankWithinSpecies() noexcept;
            void removeGenomesFromSpecies(const std::size_t& speciesID) noexcept;
            std::size_t getPopulationSize() const noexcept;
            std::size_t getSpeciesSize() const noexcept;
            JsonBox::Value toJson() const noexcept;
            void spawn(const std::size_t& numInputs, const std::size_t& numOutputs, bool canBeRecurrent = false, bool cppn = false) noexcept;
            void writeToFile(const std::string& filename);
            ~Population() = default;
        private:
            std::vector<std::unique_ptr<Genome>> genomes;
            std::vector<std::unique_ptr<Species>> species;
            std::size_t PopulationSize;
    };
}

#endif // EVOAI_POPULATION_HPP