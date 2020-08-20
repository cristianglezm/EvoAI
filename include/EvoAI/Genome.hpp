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
            /**
             *  @brief helper struct for reproduce, getDisjointGenes, getExcessGenes, getMatching*
             */
            template<class T, class InputIt = typename std::vector<T>::const_iterator>
            struct Range{
                Range(InputIt begin, InputIt end)
                : begin(begin)
                , end(end){}
                std::size_t size() const{
                    return std::distance(begin, end);
                }
                const T& operator[](const std::size_t& index) const noexcept{
                    return *(begin + index);
                }
                using iterator = typename std::vector<T>::iterator;
                using const_iterator = typename std::vector<T>::const_iterator;
                InputIt begin;
                InputIt end;
            };
            using matchingNodeGenes = std::pair<Range<NodeGene>, Range<NodeGene>>;
            using matchingConnectionGenes = std::pair<Range<ConnectionGene>, Range<ConnectionGene>>;
            using matchingChromosomes = std::pair<matchingNodeGenes, matchingConnectionGenes>;
            using excessNodeGenes = std::pair<Range<NodeGene>, Range<NodeGene>>;
            using excessConnectionGenes = std::pair<Range<ConnectionGene>, Range<ConnectionGene>>;
            using excessGenes = std::pair<excessNodeGenes, excessConnectionGenes>;
            using disjointNodeGenes = std::pair<Range<NodeGene>, Range<NodeGene>>;
            using disjointConnectionGenes = std::pair<Range<ConnectionGene>, Range<ConnectionGene>>;
            using disjointGenes = std::pair<disjointNodeGenes, disjointConnectionGenes>;
        public:
            /**
             * @brief default constructor builds an empty Genome.
             */ 
            Genome() noexcept;
            /**
             *  @brief copy constructor
             *  
             *  @param [in] rhs const Genome&
             */
            Genome(const Genome& rhs) noexcept;
            /**
             *  @brief move constructor
             *  
             *  @param [in] rhs Genome&&
             */
            Genome(Genome&& rhs) noexcept;
            /**
             * @brief builds a Genome that is feedforward connected from inputs to outputs.
             * @param numInputs
             * @param numOutputs
             * @param canBeRecursive
             * @param cppn with this true the genome will have random activation functions and will be able to change activations with Genome::mutate
             * @return Genome
             */
            Genome(const std::size_t& numInputs, const std::size_t& numOutputs, bool canBeRecursive = false, bool cppn = false) noexcept;
            /**
             * @brief builds a Genome that is feedforward connected from inputs to hidden to outputs.
             * @param numInputs
             * @param numHidden 
             * @param numOutputs
             * @param canBeRecursive
             * @param cppn with this true the genome will have random activation functions and will be able to change activations with Genome::mutate
             * @return Genome
             */
            Genome(const std::size_t& numInputs, const std::size_t& numHidden, const std::size_t& numOutputs, bool canBeRecursive = false, bool cppn = false) noexcept;
            /**
             * @brief loads a Genome JsonBox::Object
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
            std::size_t getNumOfNodes(const std::size_t& layerID) const noexcept;
            /**
             * @brief returns a Json value with the information of the genome.
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief writes the genome to a json file.
             *      it can be loaded again with the constructor.
             * @code
             *      EvoAI::Genome g(jsonfile);
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
            void setID(const std::size_t& gnmID) noexcept;
            /**
             * @brief gets the genome id
             * @return std::size_t
             */
            const std::size_t& getID() const noexcept;
            /**
             * @brief setter for species ID
             * @param spcID species id
             */
            void setSpeciesID(const std::size_t& spcID) noexcept;
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
             * @brief returns true if is a CPPN
             * @return bool
             */
            bool isCppn() const noexcept;
            /**
             * @brief setter to change RecurrentAllowed
             * @param isRecurrentAllowed
             */
            inline void setRecurrentAllowed(bool isRecurrentAllowed) noexcept{ rnnAllowed = isRecurrentAllowed; }
            /**
             * @brief returns true if it allows to have recurrent connections.
             * @return bool 
             */
            inline bool isRecurrentAllowed() const noexcept{ return rnnAllowed; }
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
             * @brief mutates the genome only once depending on rates, 
             *          the first activated is the only thing mutating.
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
        public:
            Genome& operator=(const Genome& rhs) noexcept;
            Genome& operator=(Genome&& rhs) noexcept;
            constexpr bool operator==(const Genome& rhs) const noexcept;
            constexpr bool operator<(const Genome& rhs) const noexcept;
            constexpr bool operator>(const Genome& rhs) const noexcept;
        public:
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
             * @brief It returns a matchingNodeGenes of matching NodeGenes between g1 and g2.
             * @param g1 Genome
             * @param g2 Genome
             * @return matchingNodeGenes
             */
            static matchingNodeGenes getMatchingNodeGenes(const Genome& g1, const Genome& g2) noexcept;
            /**
             * @brief It returns a MatchingConnectionGenes of matching ConnectionGenes between g1 and g2.
             * @param g1 Genome
             * @param g2 Genome
             * @return matchingConnectionGenes
             */
            static matchingConnectionGenes getMatchingConnectionGenes(const Genome& g1, const Genome& g2) noexcept;
            /**
             * @brief It returns a matchingChromosomes of matching NodeGene, ConnectionGene between g1 and g2.
             * @param g1 Genome
             * @param g2 Genome
             * @return matchingChromosomes
             */
            static matchingChromosomes getMatchingChromosomes(const Genome& g1, const Genome& g2) noexcept;
            /**
             * @brief It returns a excessGenes of g1 and g2
             * @param g1 const Genome&
             * @param g2 const Genome&
             * @param hint disjointGenes*
             * @return excessGenes
             */
            static excessGenes getExcessGenes(const Genome& g1, const Genome& g2, disjointGenes* hint = nullptr) noexcept;
            /**
             * @brief It returns a disjointGenes of g1 and g2
             * @param g1 const Genome&
             * @param g2 const Genome&
             * @param hint matchingChromosomes*
             * @return disjointGenes
             */
            static disjointGenes getDisjointGenes(const Genome& g1, const Genome& g2, matchingChromosomes* hint = nullptr) noexcept;
            /**
             * @brief Creates a new Genome from two parents, inheriting CPPN and RecurrentAllowed from any of the parents that is true.
             * if g1 and g2 are equal it will copy g1.
             * @param g1 const Genome&
             * @param g2 const Genome&
             * @return Genome
             */
            static Genome reproduce(const Genome& g1, const Genome& g2) noexcept;
            /**
             * @brief Creates a NeuralNetwork from a Genome.
             * @param g Genome
             * @return NeuralNetwork
             */
            static NeuralNetwork makePhenotype(const Genome& g) noexcept;
            /**
             * @brief Creates a genome from a neural network, it will not be exactly the same if the network wasn't made from a genome.
             * @param nn NeuralNetwork
             * @return Genome
             */
            static Genome makeGenome(NeuralNetwork& nn) noexcept;
        private:
            std::vector<NodeGene> nodeChromosomes;
            std::vector<ConnectionGene> connectionChromosomes;
            std::size_t genomeID;
            std::size_t speciesID;
            double fitness;
            bool rnnAllowed;
            bool cppn;
    };
    constexpr bool Genome::operator==(const Genome& rhs) const noexcept{
        return (genomeID == rhs.genomeID &&
                speciesID == rhs.speciesID &&
                fitness == rhs.fitness &&
                rnnAllowed == rhs.rnnAllowed &&
                cppn == rhs.cppn &&
                nodeChromosomes == rhs.nodeChromosomes &&
                connectionChromosomes == rhs.connectionChromosomes);
    }
    constexpr bool Genome::operator<(const Genome& rhs) const noexcept{
        return genomeID < rhs.genomeID;
    }
    constexpr bool Genome::operator>(const Genome& rhs) const noexcept{
        return genomeID > rhs.genomeID;
    }
}

#endif // EVOAI_GENOME_HPP