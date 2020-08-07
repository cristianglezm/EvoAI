# EvoAI #

TODO:

* Genome
    * ConnectionGene [change toString to operator<<]
* Refactor Population And Species into Population\<T\>, Species\<T\>
    * sort genomes by id
    * use equal_range / find?
    * Check requirements with a constexpr function like "is_speciable?" check for functions distance, reproduce?
    * Split Population reproduce function into smaller ones.
        - add overload to use a provided function.
    * refactor XOR, iris... to use Population<Genome> as needed
* Chromosome container?
    * using Chromosome = vector<T>
    * chromosome reproduce(c1. c2)
        - pair<Range<T>,Range<T>> getMatchingGenes(begin1, end1, begin2, end2, hint*)
        - pair<Range<T>,Range<T>> getDisjointGenes(begin1, end1, begin2, end2, hint*)
        - pair<Range<T>,Range<T>> getExcessGenes(begin1, end1, begin2, end2, hint*)
    * std::size_t distance(c1, c2, c1, c2, c3)
    * Specie<Chromosome> [specialization for Chromosome]
        - chromosome::distance
        - chromosome::reproduce

* Fix Activations and derivatives
    * add tests for activations, derivatives
* add travis
* add conan?

* think about these:
    * Check training for NeuralNetwork and fix back-propagation and gradient descend
        * Divide functions run and train into little functions that do a simple job
    * refactor NeuralNetwork
* AI algorithms
    * Decision Trees?
    * Monte Carlo tree search (MCTS)?
