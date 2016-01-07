#include <EvoAI/Neuron.hpp>

namespace EvoAI{
    Neuron::Neuron()
    : output(0.0)
    , sum(0.0)
    , delta(0.0)
    , biasWeight(1.0)
    , type(Type::HIDDEN)
    , connections(){}
    Neuron::Neuron(Type t)
    : output(0.0)
    , sum(0.0)
    , delta(0.0)
    , biasWeight(1.0)
    , type(t)
    , connections(){}
    Neuron& Neuron::setType(Type t){
        type = t;
        return *this;
    }
    Neuron& Neuron::setOutput(const double& out){
        output = out;
        return *this;
    }
    Neuron& Neuron::addSum(const double& sum){
        this->sum += sum;
        return *this;
    }
    Neuron& Neuron::setSum(const double& sum){
        this->sum = sum;
        return *this;
    }
    Neuron& Neuron::setDelta(const double& delta){
        this->delta = delta;
        return *this;
    }
    Neuron& Neuron::reset(){
       if(type != Type::CONTEXT){
            output = 0.0;
            sum = 0.0;
            delta = 0.0;
       }
       return *this;
    }
    Neuron& Neuron::resetContext(){
        output = 0.0;
        sum = 0.0;
        delta = 0.0;
        return *this;
    }
    JsonBox::Value Neuron::toJson(){
        using namespace JsonBox;
        Object o;
        o["output"] = Value(output);
        o["sum"] = Value(sum);
        o["delta"] = Value(delta);
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
    std::string Neuron::toString(){
        std::ostringstream os;
        os << "output: " << output << ", sum: " << sum << 
              ", delta: " << delta << ", biasWeight: " << biasWeight << ", type: ";
        switch(type){
            case Type::CONTEXT: os << "context";   break;
            case Type::HIDDEN:  os << "hidden";    break;
            case Type::INPUT:   os << "input";     break;
            case Type::OUTPUT:  os << "output";    break;
            default:            os << "undefined"; break;
        }
        return os.str();
    }
    Neuron& Neuron::addConnection(const Connection& c){
        connections.emplace_back(c);
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
        return (output == rhs.output
                && sum == rhs.sum
                && delta == rhs.delta
                && type == rhs.type);
    }
    Neuron& Neuron::setBiasWeight(const double& bw){
        biasWeight = bw;
        return *this;
    }
}
