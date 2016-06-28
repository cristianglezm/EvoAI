#ifndef EVOAI_SPECIES_HPP
#define EVOAI_SPECIES_HPP

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
     * @class Species
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief Class that represents a species
     */
    class Species{
        public:
            Species();
            Species(std::size_t id, bool novel);
            Species(JsonBox::Object o);
            Species(const std::string& filename);
            void adjustFitness() noexcept;
            void computeAvgFitness() noexcept;
            void computeMaxFitness() noexcept;
            void rank() noexcept;
            Genome* getRepresentative() const noexcept;
            Genome* getChampion() noexcept;
            bool isNovel() const noexcept;
            void setNovel(bool n) noexcept;
            bool isKillable() const noexcept;
            void setKillable(bool k) noexcept;
            void setID(const std::size_t& speciesID) noexcept;
            const std::size_t& getID() const noexcept;
            void setAge(const std::size_t& speciesAge) noexcept;
            void addAge(const std::size_t& amount) noexcept;
            const std::size_t& getAge() const noexcept;
            void setGenomes(std::vector<std::unique_ptr<Genome>>&& gs) noexcept;
            void addGenome(std::unique_ptr<Genome>&& g) noexcept;
            void removeGenome(Genome* g) noexcept;
            bool has(Genome* g) noexcept;
            std::vector<std::unique_ptr<Genome>>& getGenomes() noexcept;
            const double& getAvgFitness() const noexcept;
            const double& getMaxFitness() const noexcept;
            std::size_t getSize() const noexcept;
            void setNumOffsprings(std::size_t& numOff) noexcept;
            const std::size_t& getNumOffsprings() const noexcept;
            JsonBox::Value toJson() const noexcept;
            void writeToFile(const std::string& filename) const noexcept;
            ~Species() = default;
        private:
            std::size_t id;
            std::size_t age;
            bool novel;
            bool killable;
            double avgFitness;
            double maxFitness;
            double oldAvgFitness;
            std::size_t numOffsprings;
            std::vector<std::unique_ptr<Genome>> genomes;
    };
}

#endif // EVOAI_SPECIES_HPP