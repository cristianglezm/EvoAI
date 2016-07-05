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
     * @class Population
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief Population
     */
    class Population{
        public:
            /**
             * @brief Types of selection for reproduce.
             */
            enum SelectionType{
                TRUNCATION,
                TOURNAMENT,
                FPS
                };
        public:
            /**
             * @brief basic constructor
             * @return Population
             */
            Population();
            /**
             * @brief creates a population of size with the number of inputs and outputs.
             * @param size const std::size_t& number of individuals
             * @param numInputs const std::size_t& inputs of the genomes
             * @param numOutputs const std::size_t& outputs of the genomes
             * @param canBeRecurrent bool let genomes mutate recursive connections.
             * @param cppn bool let genomes mutate their activations functions.
             * @return Population
             */
            Population(const std::size_t& size, const std::size_t& numInputs, const std::size_t& numOutputs, bool canBeRecurrent = false, bool cppn = false);
            /**
             * @brief creates a population of size with the number of inputs and outputs.
             * @param size const std::size_t& number of individuals.
             * @param numInputs const std::size_t& inputs neurons of the genomes.
             * @param numhidden const std::size_t& hidden neurons of the genomes.
             * @param numOutputs const std::size_t& outputs neurons of the genomes.
             * @param canBeRecurrent bool let genomes mutate recursive connections.
             * @param cppn bool let genomes mutate their activations functions.
             * @return Population
             */ 
            Population(const std::size_t& size, const std::size_t& numInputs, const std::size_t& numHidden, const std::size_t& numOutputs, bool canBeRecurrent = false, bool cppn = false);
            /**
             * @brief Constructor for JsonBox::Object
             * @param o JsonBox::Object
             * @return Population
             */
            Population(JsonBox::Object o);
            /**
             * @brief Constructor to load a Population from a json file.
             * @param filename loads a json file written with Population::writeTofile
             * @return Population
             */
            Population(const std::string& filename);
            /**
             * @brief Adds a genome and checks species if it not compatible with any makes a new species.
             * @param g std::unique_ptr<Genome>
             */
            void addGenome(std::unique_ptr<Genome> g) noexcept;
            /**
             * @brief removes a Genome.
             * @param g Genome*
             */
            void removeGenome(Genome* g) noexcept;
            /**
             * @brief adds a Species
             * @param sp std::unique_ptr<Species>
             */
            void addSpecies(std::unique_ptr<Species> sp) noexcept;
            /**
             * @brief removes a Species
             * @param sp Species*
             */
            void removeSpecies(Species* sp) noexcept;
            /**
             * @brief finds a genome, returns nullptr if not found
             * @param id const std::size_t& genome ID
             * @return Genome* or nullptr if not found
             */
            Genome* findGenome(const std::size_t& id) noexcept;
            /**
             * @brief gets the best genome it has in the Population.
             * @return Genome* or nullptr if not found.
             */
            Genome* getBestGenome() noexcept;
            /**
             * @brief finds a Species, returns nullptr if not found.
             * @param id const std::size_t& Species ID
             * @return Species* or nullptr if not found.
             */
            Species* findSpecies(const std::size_t& id) noexcept;
            /**
             * @brief returns the population.
             * @return std::vector<Genome*>&
             */
            std::vector<Genome*>& getGenomes() noexcept;
            /**
             * @brief returns the Species
             * @return std::vector<std::unique_ptr<Species>>&
             */
            std::vector<std::unique_ptr<Species>>& getSpecies() noexcept;
            /**
             * @brief removes species older than Population::maxAge
             * increses the age by one.
             */
            void removeOldSpecies() noexcept;
            /**
             * @brief Replaces the less capable members of the Population with the children of the best.
             * @param interSpecies if is permitted to reproduce between other species.
             * @param st Population::SelectionType
             */
            void reproduce(bool interSpecies = false, SelectionType st = SelectionType::TOURNAMENT) noexcept;
            /**
             * @brief orders the genomes from the population by fitness
             */
            void orderGenomesByFitness() noexcept;
            /**
             * @brief orders the species by age
             */
            void orderSpeciesByAge() noexcept;
            /**
             * @brief orders the species by fitness
             */
            void orderSpeciesByFitness() noexcept;
            /**
             * @brief Orders all the genomes inside the species.
             */
            void rankWithinSpecies() noexcept;
            /**
             * @brief returns the population Size.
             * @return std::size_t
             */
            std::size_t getPopulationSize() const noexcept;
            /**
             * @brief return number of species.
             * @return std::size_t
             */
            std::size_t getSpeciesSize() const noexcept;
            /**
             * @brief setter for maxAge
             * @param age std::size_t
             */
            void setMaxAge(std::size_t age) noexcept;
            /**
             * @brief returns maxAge
             * @param const std::size_t&
             */
            const std::size_t& getMaxAge() const noexcept;
            /**
             * @brief setter for the compability Threehold
             * @param compThreshold const double&
             */
            void setCompatibilityThreshold(const double& compThreshold) noexcept;
            /**
             * @brief getter for the compability Threshold
             * @return const double&
             */
            const double& getCompatibilityThreshold() const noexcept;
            /**
             * @brief computes the average fitness of the Population.
             * @return double
             */
            double computeAvgFitness() noexcept;
            /**
             * @brief returns a JsonBox::Value with the current info.
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief Writes the Population to a json file.
             * @param filename const std::string&
             */
            void writeToFile(const std::string& filename);
            ~Population() = default;
        private:
            static std::size_t getNewID() noexcept;
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