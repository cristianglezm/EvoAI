#include <EvoAI/Neuron.hpp>

namespace EvoAI{
    Neuron::Neuron()
    : value(0.0)
    , oldValue(0.0)
    , error(0.0)
    , type(Neuron::HIDDEN){}
    Neuron::Neuron(const Type& t)
    : value(0.0)
    , oldValue(0.0)
    , error(0.0)
    , type(t){}
    Neuron& Neuron::setType(const Type& t){
        type = t;
        return *this;
    }
    Neuron& Neuron::setValue(const double& val){
        oldValue = value;
        value = val;
        return *this;
    }
    Neuron& Neuron::setError(const double& err){
        error = err;
        return *this;
    }
    Neuron& Neuron::reset(){
       if(type != Neuron::CONTEXT){
            value = 0.0;
            oldValue = 0.0;
            error = 0.0;
       }
       return *this;
    }
}
