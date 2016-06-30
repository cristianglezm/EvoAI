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
    class Population{
        public:
            enum SelectionType{
                TRUNCATION,
                TOURNAMENT,
                FPS,
                SUS
                };
        public:
            Population();
            Population(const std::size_t& size, const std::size_t& numInputs, const std::size_t& numOutputs, bool canBeRecurrent = false, bool cppn = false);
            Population(const std::size_t& size, const std::size_t& numInputs, const std::size_t& numHidden, const std::size_t& numOutputs, bool canBeRecurrent = false, bool cppn = false);
            Population(JsonBox::Object o);
            Population(const std::string& filename);
            void addGenome(std::unique_ptr<Genome> g) noexcept;
            void removeGenome(Genome* g) noexcept;
            void addSpecies(std::unique_ptr<Species> sp) noexcept;
            void removeSpecies(Species* sp) noexcept;
            Genome* findGenome(const std::size_t& id) noexcept;
            Genome* getBestGenome() noexcept;
            Species* findSpecies(const std::size_t& id) noexcept;
            std::vector<Genome*>& getGenomes() noexcept;
            std::vector<std::unique_ptr<Species>>& getSpecies() noexcept;
            void removeOldSpecies() noexcept;
            void reproduce(bool interSpecies = false, SelectionType st = SelectionType::TOURNAMENT) noexcept;
            void orderGenomesByFitness() noexcept;
            void orderSpeciesByAge() noexcept;
            void orderSpeciesByFitness() noexcept;
            void rankWithinSpecies() noexcept;
            std::size_t getPopulationSize() const noexcept;
            std::size_t getSpeciesSize() const noexcept;
            std::size_t getNewID() const noexcept;
            void setMaxAge(std::size_t age) noexcept;
            const std::size_t& getMaxAge() const noexcept;
            void setCompatibilityThreshold(const double& compThreshold) noexcept;
            const double& getCompatibilityThreshold() const noexcept;
            JsonBox::Value toJson() const noexcept;
            void writeToFile(const std::string& filename);
            ~Population() = default;
        private:
            std::vector<std::unique_ptr<Species>> species;
            std::vector<Genome*> genomes;
            std::size_t PopulationSize;
            bool genomesCached;
            double compatibilityThreshold;
            std::size_t maxAge;
    };
}

#endif // EVOAI_POPULATION_HPP