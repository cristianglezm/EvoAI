#include <EvoAI/Genome.hpp>

namespace EvoAI{
    Genome::Genome()
    {}
    Genome::Genome(const std::size_t& chromoSize)
    {}
    bool Genome::update(){
        return false;
    }
    JsonBox::Value Genome::toJson(){
        return JsonBox::Value();
    }
//private
    Genome& Genome::addConnection(){
        return *this;
    }
    Genome& Genome::removeConnection(){
        return *this;
    }
    Genome& Genome::addNeuron(){
        return *this;
    }
    Genome& Genome::removeNeuron(){
        return *this;
    }
    Genome& Genome::addNeuronLayer(){
        return *this;
    }
    Genome& Genome::removeNeuronLayer(){ 
        return *this;
    }
    Genome& Genome::addChromosome(const double& chromo){ 
        return *this;
    }
    Genome& Genome::removeChromosome(const double& chromo){
        return *this;
    }
    bool Genome::initChromosomes(){
        return false;
    }
}
