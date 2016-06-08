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
      * @todo
      * Shared fitness with species
      * list of the innovations that occurred in the current generation
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
            const std::size_t& getGenomeID() const noexcept;
            void setSpeciesID(std::size_t spcID) noexcept;
            const std::size_t& getSpeciesID() const noexcept;
            void mutateAddNode() noexcept;
            void mutateAddConnection() noexcept;
            /// look at src genome NEAT
            void mutateWeights(double power) noexcept;
            /**
             * @brief finds a connectionGene that is not enabled and enables it.
             */
            void mutateEnable() noexcept;
            /// will mutate adding a node + 2 connection or a connection between 2 existing nodes.
            /// @todo add mutationRates Info?
            void mutate() noexcept;
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
                                    const double& c1 = 0.4, 
                                    const double& c2 = 0.3, 
                                    const double& c3 = 0.1) noexcept;
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
            static excessGenes getExcessGenes(const Genome& g1, const Genome& g2) noexcept;
            static disjointGenes getDisjointGenes(const Genome& g1, const Genome& g2) noexcept;
            /// genes are randomly chosen from either parent at matching genes,
            /// whereas all excess or disjoint genes are always included from the more fit parent
            /// there’s a preset chance that an inherited gene is disabled if it is disabled in either parent
            static std::unique_ptr<Genome> reproduce(const Genome& g1, const Genome& g2) noexcept;
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

#endif // #ifndef EVOAI_GENOME_HPP