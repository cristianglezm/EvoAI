#ifndef GENOME_HPP
#define GENOME_HPP

#include <vector>

namespace EvoAI{
    class Genome final{
        public:
            Genome();

            ~Genome() = default;
        private:
		//initial values
		// funciones de la nn modifica values hidden layers, hidden neurons
		// species size chromosomas
		// limite cost function
		// serializar nn, genome
            std::vector<double> chromosomas;
            std::unique_ptr<NeuralNetwork> nn;
	    
    };
}
#endif // GENOME_HPP

/*
	Genome is like a graph that describes dimensions aka number of input, outputs and layers
	geolocation, weights from gaussean function?
	something like data[input*outputs]
	+---------------------+ layers?
	|3|4|5|6|3|3|-7|-1|4|3| +-------+
	+---------------------+ |3|4|5|6|
	|1|3|5|5|6|2|-3|-2|5|6| +-------+
	+---------------------+
	|4|5|7|2|3|4|-2|-7|1|4| x|l,y|l connection geo y,l|x connection weight
	+---------------------+

*/
