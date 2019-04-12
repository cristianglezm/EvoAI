#ifndef EVOAI_GENOME_HPP
#define EVOAI_GENOME_HPP

#include <JsonBox.h>
#include <EvoAI/NodeGene.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/ConnectionGene.hpp>
#include <EvoAI/Utils.hpp>
#include <memory>
#include <chrono>
#include <functional>

#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @class Genome
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief A genome is a description of a NeuralNetwork.
     */
    class EvoAI_API Genome final{
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
            /**
             * @brief default constructor builds an empty Genome.
             */ 
            Genome();
            /**
             * @brief builds a Genome that is feedforward connected from inputs to outputs.
             * @param numInputs
             * @param numOutputs
             * @param canBeRecursive
             * @param cppn with this true the genome will have random activation functions and will be able to change activations with Genome::mutate
             * @return Genome
             */
            Genome(std::size_t numInputs, std::size_t numOutputs, bool canBeRecursive = true, bool cppn = false);
            /**
             * @brief builds a Genome that is feedforward connected from inputs to hidden to outputs.
             * @param numInputs
             * @param numHidden 
             * @param numOutputs
             * @param canBeRecursive
             * @param cppn with this true the genome will have random activation functions and will be able to change activations with Genome::mutate
             * @return Genome
             */
            Genome(std::size_t numInputs, std::size_t numHidden, std::size_t numOutputs, bool canBeRecursive = true, bool cppn = false);
            /**
             * @brief loads a Genome previously saved with Genome::toJson
             * @param o JsonBox::Object
             * @return Genome
             */
            Genome(JsonBox::Object o);
            /**
             * @brief loads a Genome from a jsonfile previously saved with Genome::writeToFile
             * @param jsonfile std::string&
             * @return Genome
             */
            Genome(const std::string& jsonfile);
            /**
             * @brief adds a NodeGene
             * @param ng NodeGene
             */
            void addGene(const NodeGene& ng) noexcept;
            /**
             * @brief adds a ConnectionGene
             * @param cg ConnectionGene
             */
            void addGene(const ConnectionGene& cg) noexcept;
            /**
             * @brief setter for the NodeGene.
             * @param ngenes std::vector<NodeGene>&&
             */
            void setNodeChromosomes(std::vector<NodeGene>&& ngenes) noexcept;
            /**
             * @brief getter for NodeGenes
             * @return std::vector<NodeGene>&
             */
            std::vector<NodeGene>& getNodeChromosomes() noexcept;
            /**
             * @brief const getter for NodeGenes
             * @return const std::vector<NodeGene>&
             */
            const std::vector<NodeGene>& getNodeChromosomes() const noexcept;
            /**
             * @brief setter for the ConnectionGene
             * @param cgenes std::vector<ConnectionGene>&&
             */
            void setConnectionChromosomes(std::vector<ConnectionGene>&& cgenes) noexcept;
            /**
             * @brief getter for the connectionGenes
             * @return std::vector<ConnectionGene>&
             */
            std::vector<ConnectionGene>& getConnectionChromosomes() noexcept;
            /**
             * @brief const getter for the connectionGenes
             * @return const std::vector<ConnectionGene>&
             */
            const std::vector<ConnectionGene>& getConnectionChromosomes() const noexcept;
            /**
             * @brief gets the number of nodes from a layer.
             * @param layerID count the nodes from that layer.
             * @return std::size_t number of nodes
             */
            std::size_t getNumOfNodes(std::size_t layerID) const noexcept;
            /**
             * @brief returns a Json value with the information of the genome.
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief writes the genome to a json file.
             * it can be loaded again with the constructor
             * @code
             * EvoAI::Genome g(jsonfile);
             * @endcode
             * @param filename std::string file to write
             */
            void writeToFile(const std::string& filename) const noexcept;
            /**
             * @brief gets the genome id
             * @param fit double
             */
            void setFitness(const double& fit) noexcept;
            /**
             * @brief adds the amount to the current fitness.
             * @param amount const double&
             */
            void addFitness(const double& amount) noexcept;
            /**
             * @brief gets the fitness
             * @return double
             */
            const double& getFitness() const noexcept;
            /**
             * @brief Checks if it has the NodeGene.
             * @param ng NodeGene
             * @return bool
             */
            bool hasNodeGene(const NodeGene& ng) const noexcept;
            /**
             * @brief Checks if it has the ConnectionGene
             * @param cg ConnectionGene
             * @return bool true if it has the connection gene.
             */
            bool hasConnectionGene(const ConnectionGene& cg) const noexcept;
            /**
             * @brief sets the genome id
             * @param gnmID std::size_t
             */
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
             * @brief setter for cppn
             * @param bool isCppn
             */
            void setCppn(bool isCppn) noexcept;
            /**
             * @brief returns if is cppn
             * @return const bool
             */
            const bool isCppn() const noexcept;
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
             * @brief selects a random connectionGene that is enabled and disables it
             */
            void mutateDisable() noexcept;
            /**
             * @brief selects a random connectionGene that is not enabled and enables it.
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
             * @param disableRate float
             * @param actTypeRate float
             */
            void mutate(float nodeRate = 0.2, float addConnRate = 0.3, float removeConnRate = 0.2,
                                        float perturbWeightsRate = 0.6, float enableRate = 0.35,
                                        float disableRate = 0.3, float actTypeRate = 0.4) noexcept;
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
             * if g1 and g2 are equal it will copy g1.
             * @param g1 const Genome&
             * @param g2 const Genome&
             * @return std::unique_ptr<Genome>
             */
            static std::unique_ptr<Genome> reproduce(const Genome& g1, const Genome& g2) noexcept;
            /**
             * @brief Creates a NeuralNetwork from a Genome.
             * @param g Genome
             * @return std::unique_ptr<NeuralNetwork>
             */
            static std::unique_ptr<NeuralNetwork> makePhenotype(const Genome& g) noexcept;
            /**
             * @brief Creates a genome from a neural network, it will not be exactly the same if the network wasn't made from a genome.
             * @param nn NeuralNetwork
             * @return std::unique_ptr<Genome>
             */
            static std::unique_ptr<Genome> makeGenome(NeuralNetwork& nn) noexcept;
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
