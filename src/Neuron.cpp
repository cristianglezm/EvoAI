#include <EvoAI/Neuron.hpp>

namespace EvoAI{
    Neuron::Neuron()
    : value(0.0)
    , oldValue(0.0)
    , error(0.0)
    , type(Type::HIDDEN){}
    Neuron::Neuron(Type t)
    : value(0.0)
    , oldValue(0.0)
    , error(0.0)
    , type(t){}
    Neuron& Neuron::setType(Type t){
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
       if(type != Type::CONTEXT){
            value = 0.0;
            oldValue = 0.0;
            error = 0.0;
       }
       return *this;
    }
    Neuron& Neuron::resetContext(){
        value = 0.0;
        oldValue = 0.0;
        error = 0.0;
        return *this;
    }
    JsonBox::Value Neuron::toJson(){
        using namespace JsonBox;
        Object o;
        o["value"] = Value(value);
        o["oldValue"] = Value(oldValue);
        o["error"] = Value(error);
        o["biasWeight"] = Value(biasWeight);
        switch(type){
            case Type::CONTEXT: o["type"] = Value("context");   break;
            case Type::HIDDEN:  o["type"] = Value("hidden");    break;
            case Type::INPUT:   o["type"] = Value("input");     break;
            case Type::OUTPUT:  o["type"] = Value("output");    break;
            default:            o["type"] = Value("undefined"); break;
        }
        Array a;
        for(auto& c:connections){
            a.push_back(c.toJson());
        }
        o["connections"] = Value(a);
        return Value(o);
}
    Neuron& Neuron::addConnection(const Connection& c){
        connections.push_back(c);
        return *this;
    }
    bool Neuron::removeConnection(const Connection& c){
        auto removed = std::remove_if(std::begin(connections),std::end(connections),
                                      [&c](const Connection& rc){
                                            return (c == rc);
                                      });
        connections.erase(removed,std::end(connections));
        return (removed == std::end(connections));
    }
    bool Neuron::operator==(const Neuron& rhs) const{
        return (value == rhs.value
                && oldValue == rhs.oldValue
                && error == rhs.error
                && type == rhs.type);
    }
    Neuron& Neuron::setBiasWeight(const double& bw){
        biasWeight = bw;
        return *this;
    }
}
