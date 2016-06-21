#ifndef EVOAI_GENOME_HPP
#define EVOAI_GENOME_HPP

#include <JsonBox.h>
#include <EvoAI/NodeGene.hpp>
#include <EvoAi/NeuralNetwork.hpp>
#include <EvoAI/ConnectionGene.hpp>
#include <EvoAI/Utils.hpp>
#include <memory>
#include <chrono>
#include <functional>

namespace EvoAI{
    /**
     * @class Genome
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief A genome is a description of a NeuralNetwork.
     */
    class Genome final{
        public:
            using matchingNodeGenes = std::pair<std::vector<NodeGene>, std::vector<NodeGene>>;
            using matchingConnectionGenes = std::pair<std::vector<ConnectionGene>, std::vector<ConnectionGene>>;
            using matchingChromosomes = std::pair<matchingNodeGenes, matchingConnectionGenes>;
            using excessNodeGenes = std::vector<NodeGene>;
            using excessConnectionGenes = std::vector<ConnectionGene>;
            using excessGenes = std::pair<excessNodeGenes, excessConnectionGenes>;
            using disjointNodeGenes = std::vector<NodeGene>;
            using disjointConnectionGenes = std::vector<ConnectionGene>;
            using disjointGenes = std::pair<disjointNodeGenes, disjointConnectionGenes>;
        public:
            Genome();
            Genome(std::size_t numInputs, std::size_t numOutputs, bool canBeRecursive = true, bool cppn = false);
            Genome(std::size_t numInputs, std::size_t numHidden, std::size_t numOutputs, bool canBeRecursive = true, bool cppn = false);
            Genome(JsonBox::Object o);
            Genome(const std::string& jsonfile);
            void addGene(const NodeGene& ng) noexcept;
            void addGene(const ConnectionGene& cg) noexcept;
            void setNodeChromosomes(std::vector<NodeGene>&& ngenes) noexcept;
            std::vector<NodeGene>& getNodeChromosomes() noexcept;
            void setConnectionChromosomes(std::vector<ConnectionGene>&& cgenes) noexcept;
            std::vector<ConnectionGene>& getConnectionChromosomes() noexcept;
            std::size_t getNumOfNodes(std::size_t layerID) const noexcept;
            JsonBox::Value toJson() noexcept;
            void writeToFile(const std::string& filename) noexcept;
            void setFitness(const double& fit) noexcept;
            const double& getFitness() const noexcept;
            bool hasNodeGene(const NodeGene& ng) const noexcept;
            bool hasConnectionGene(const ConnectionGene& cg) const noexcept;
            void setGenomeID(std::size_t gnmID) noexcept;
            /**
             * @brief gets the genome id
             * @return std::size_t
             */
            const std::size_t& getGenomeID() const noexcept;
            /**
             * @brief setter for species ID
             * @param spcID species id
             */
            void setSpeciesID(std::size_t spcID) noexcept;
            /**
             * @brief returns the species id.
             * @return std::size_t&
             */
            const std::size_t& getSpeciesID() const noexcept;
            /**
             * @brief Adds a node and random connection and slices the connection adding the node in between.
             */
            void mutateAddNode() noexcept;
            /**
             * @brief Adds a random connection between two nodeGenes.
             */
            void mutateAddConnection() noexcept;
            /**
             * @brief Removes a random connection between two nodeGenes.
             */
            void mutateRemoveConnection() noexcept;
            /**
             * @brief mutates the weights of a NodeGene or ConnectionGene.
             * @param power
             */
            void mutateWeights(double power) noexcept;
            /**
             * @brief finds a connectionGene that is not enabled and enables it.
             */
            void mutateEnable() noexcept;
            /**
             * @brief selects a random nodeGene and changes it activation function.
             */
            void mutateActivationType() noexcept;
            /**
             * @brief mutates the genome
             * Rates 0.0-1.0
             * @param nodeRate float
             * @param connectionRate float
             * @param perturbWeightsRate float
             * @param enableRate float
             */
            void mutate(float nodeRate = 0.2, float addConnRate = 0.3, float removeConnRate = 0.2, float perturbWeightsRate = 0.6, float enableRate = 0.4, float actTypeRate = 0.4) noexcept;
            /**
             * @brief Checks if the genome is valid.
             * @return bool true if all is ok 
             */
            bool isValid() noexcept;
            ~Genome() = default;
            /**
             * @brief returns a random ActivationType.
             * @return Neuron::ActivationType
             */
            static Neuron::ActivationType getRandomActivationType() noexcept;
            /**
             * @brief Calculates the distance between two genomes.
             * @param g1 Genome
             * @param g2 Genome
             * @param c1 coefficient Gives importance to Excess Genes 
             * @param c2 coefficient Gives importance to Disjoints Genes
             * @param c3 coefficient Gives importance to Weight Differences
             * @return double
             */
            static double distance(const Genome& g1, const Genome& g2, 
                                    const double& c1 = 2.0, 
                                    const double& c2 = 2.0, 
                                    const double& c3 = 1.0) noexcept;
            /**
             * @brief returns the matching NodeGenes between two genomes.
             * @param g1 Genome
             * @param g2 Genome
             * @return matchingNodeGenes
             */
            static matchingNodeGenes getMatchingNodeGenes(const Genome& g1, const Genome& g2) noexcept;
            /**
             * @brief returns the matching ConnectionGenes between two genomes.
             * @param g1 Genome
             * @param g2 Genome
             * @return matchingConnectionGenes
             */
            static matchingConnectionGenes getMatchingConnectionGenes(const Genome& g1, const Genome& g2) noexcept;
            /**
             * @brief returns the matching matchingChromosomes between two genomes
             * @param g1 Genome
             * @param g2 Genome
             * @return matchingChromosomes
             */
            static matchingChromosomes getMatchingChromosomes(const Genome& g1, const Genome& g2) noexcept;
            /**
             * @brief returns Excess genes of g1
             * @param g1 const Genome&
             * @param g2 const Genome&
             * @return excessGenes
             */
            static excessGenes getExcessGenes(const Genome& g1, const Genome& g2) noexcept;
            /**
             * @brief returns Disjoint genes of g1
             * @param g1 const Genome&
             * @param g2 const Genome&
             * @return disjointGenes
             */
            static disjointGenes getDisjointGenes(const Genome& g1, const Genome& g2) noexcept;
            /**
             * @brief Creates a new Genome from two parents.
             * @param g1 const Genome&
             * @param g2 const Genome&
             * @return std::unique_ptr<Genome>
             */
            static std::unique_ptr<Genome> reproduce(const Genome& g1, const Genome& g2) noexcept;
            /**
             * @brief Creates a NeuralNetwork from a Genomme.
             * @param g Genome
             * @return std::unique_ptr<NeuralNetwork>
             */
            static std::unique_ptr<NeuralNetwork> makePhenotype(Genome& g) noexcept;
        private:
            std::size_t genomeID;
            std::size_t speciesID;
            double fitness;
            bool rnnAllowed;
            bool cppn;
            std::vector<NodeGene> nodeChromosomes;
            std::vector<ConnectionGene> connectionChromosomes;
    };
}

#endif // EVOAI_GENOME_HPP