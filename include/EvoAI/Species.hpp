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
            /**
             * @brief basic Constructor
             * @return Species
             */
            Species();
            /**
             * @brief Creates a Species With the id and novel.
             * @param id std::size_T
             * @param novel bool if the species is a new one.
             * @return Species
             */
            Species(std::size_t id, bool novel);
            /**
             * @brief loads a Species from a JsonBox::Object
             * @param o JsonBox::Object
             * @return Species
             */
            Species(JsonBox::Object o);
            /**
             * @brief Loads the species from a jsonfile saved with Species::writeToFile
             * @param filename const std::string&
             * @return Species
             */
            Species(const std::string& filename);
            /**
             * @brief Adjust the fitness of the species
             */
            void adjustFitness() noexcept;
            /**
             * @brief computes the avg fitness
             */
            void computeAvgFitness() noexcept;
            /**
             * @brief computes the max fitness
             */
            void computeMaxFitness() noexcept;
            /**
             * @brief orders the genomes.
             */
            void rank() noexcept;
            /**
             * @brief returns the first member of this species
             * @return Genome*
             */
            Genome* getRepresentative() const noexcept;
            /**
             * @brief returns the best performing genome of the species.
             * @return Genome*
             */
            Genome* getChampion() noexcept;
            /**
             * @brief returns if is novel.
             * @return bool
             */
            bool isNovel() const noexcept;
            /**
             * @brief setter for novel
             * @param n bool
             */
            void setNovel(bool n) noexcept;
            /**
             * @brief returns if is killable
             * @return bool
             */
            bool isKillable() const noexcept;
            /**
             * @brief setter for killable
             * @param k bool killable
             */
            void setKillable(bool k) noexcept;
            /**
             * @brief sets the ID of the species.
             * @param speciesID const std::size_t&
             */
            void setID(const std::size_t& speciesID) noexcept;
            /**
             * @brief returns Species ID.
             */
            const std::size_t& getID() const noexcept;
            /**
             * @brief setter for the age of the species.
             * @param speciesAge const std::size_t&
             */
            void setAge(const std::size_t& speciesAge) noexcept;
            /**
             * @brief adds the amount to the current age of the species.
             * @param amount const std::size_t&
             */
            void addAge(const std::size_t& amount) noexcept;
            /**
             * @brief returns the current age of the species.
             */
            const std::size_t& getAge() const noexcept;
            /**
             * @brief setter for genomes.
             */
            void setGenomes(std::vector<std::unique_ptr<Genome>>&& gs) noexcept;
            /**
             * @brief adds a genome to the species.
             * @param g std::unique_ptr<Genome>&&
             */
            void addGenome(std::unique_ptr<Genome>&& g) noexcept;
            /**
             * @brief removes a genome from the species.
             * @param g Genome*
             */
            void removeGenome(Genome* g) noexcept;
            /**
             * @brief checks if this genome is in this species.
             * @param g Genome*
             * @return bool
             */
            bool has(Genome* g) noexcept;
            /**
             * @brief returns the genomes from this species.
             * @return std::vector<std::unique_ptr<Genome>&
             */
            std::vector<std::unique_ptr<Genome>>& getGenomes() noexcept;
            /**
             * @brief returns the avg fitness of the species.
             * @return const double&
             */
            const double& getAvgFitness() const noexcept;
            /**
             * @brief returns the max fitness that it has.
             * @return const double&
             */
            const double& getMaxFitness() const noexcept;
            /**
             * @brief returns the number of genomes.
             * @return std::size_t
             */
            std::size_t getSize() const noexcept;
            /**
             * @brief checks if has stopped getting better
             * needs to have Species::computeAvgFitness run first.
             * @return bool
             */
            bool isStagnant() const noexcept;
            /**
             * @brief returns a JsonBox::Value with the current info.
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief writes the Species to a file
             * @param filename
             */
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
            std::vector<std::unique_ptr<Genome>> genomes;
    };
}

#endif // EVOAI_SPECIES_HPP