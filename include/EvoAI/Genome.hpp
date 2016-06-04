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
     /*
      * @todo
      * Shared fitness with species
      * list of the innovations that occurred in the current generation
      */
    class Genome final{
        public:
            using matchingChromosomes = std::pair<std::pair<std::vector<NodeGene>, std::vector<NodeGene>>, 
                                                  std::pair<std::vector<ConnectionGene>, std::vector<ConnectionGene>>>;
        public:
            Genome(){}
            Genome(std::size_t numInputs, std::size_t numOutputs, bool canBeRecursive = true, bool cppn = false);
            /// @todo node is added, where an old connection was,
            ///       then the src to new node weight is 1 and the
            ///       new node to dest is equal to the old weight.
            /// *--*
            /// *-w = 1-*(new node)-old w-*
            void addGene(const NodeGene& ng) noexcept;
            void addGene(const ConnectionGene& cg) noexcept;
            void setNodeChromosomes(std::vector<NodeGene>&& ngenes) noexcept;
            std::vector<NodeGene>& getNodeChromosomes() noexcept;
            void setConnectionChromosomes(std::vector<ConnectionGene>&& cgenes) noexcept;
            std::vector<ConnectionGene>& getConnectionChromosomes() noexcept;
            JsonBox::Value toJson() noexcept;
            std::size_t getLastInnovationNode() const noexcept;
            std::size_t getLastInnovationConnection() const noexcept;
            /// will mutate adding a node + 2 connection or a connection between 2 existing nodes.
            void mutate() noexcept;
            /// look at src genome NEAT
            void mutateWeights(double power,double rate) noexcept;
            /**
             * @brief finds a connectionGene that is not enabled and enables it.
             */
            void mutateEnable() noexcept;
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
             * @brief returns all the matching genes between two genomes
             * @param g1
             * @param g2
             * @return matchingChromosomes
             */
            static matchingChromosomes getMatchingChromosomes(const Genome& g1, const Genome& g2) noexcept;
            /// genes are randomly chosen from either parent at matching genes,
            /// whereas all excess or disjoint genes are always included from the more fit parent
            /// there’s a preset chance that an inherited gene is disabled if it is disabled in either parent
            static std::unique_ptr<Genome> reproduce(const Genome& g1, const Genome& g2) noexcept;
            static std::unique_ptr<NeuralNetwork> makePhenotype(const Genome& g) noexcept;
            static double excessGenes(const Genome& g1, const Genome& g2) noexcept;
            static double disjointGenes(const Genome& g1, const Genome& g2) noexcept;
        private:
            std::size_t genomeID;
            std::size_t speciesID;
            double fitness;
            bool rnnPermited;
            std::vector<NodeGene> nodeChromosomes;
            std::vector<ConnectionGene> connectionChromosomes;
    };
}

#endif // #ifndef EVOAI_GENOME_HPP
