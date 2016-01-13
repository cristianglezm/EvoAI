#include <EvoAI/Neuron.hpp>
#include <EVoAI/Utils.hpp>

namespace EvoAI{
    Neuron::Neuron()
    : output(0.0)
    , sum(0.0)
    , delta(0.0)
    , biasWeight(random(-1.0,1.0))
    , type(Type::HIDDEN)
    , activationType(Neuron::ActivationType::STEPPED_SIGMOID)
    , connections(){}
    Neuron::Neuron(Type t)
    : output(0.0)
    , sum(0.0)
    , delta(0.0)
    , biasWeight(random(-1.0,1.0))
    , type(t)
    , activationType(Neuron::ActivationType::STEPPED_SIGMOID)
    , connections(){}
    Neuron& Neuron::setType(Type t) noexcept{
        type = t;
        return *this;
    }
    Neuron& Neuron::setOutput(const double& out) noexcept{
        output = out;
        return *this;
    }
    Neuron& Neuron::addSum(const double& val) noexcept{
        sum += val;
        return *this;
    }
    Neuron& Neuron::setSum(const double& sum) noexcept{
        this->sum = sum;
        return *this;
    }
    Neuron& Neuron::setDelta(const double& delta) noexcept{
        this->delta = delta;
        return *this;
    }
    Neuron& Neuron::reset() noexcept{
       if(type != Type::CONTEXT){
            output = 0.0;
            sum = 0.0;
            delta = 0.0;
       }
       return *this;
    }
    Neuron& Neuron::resetContext() noexcept{
        output = 0.0;
        sum = 0.0;
        delta = 0.0;
        return *this;
    }
    JsonBox::Value Neuron::toJson() const noexcept{
        JsonBox::Object o;
        o["biasWeight"] = JsonBox::Value(biasWeight);
        o["type"] = JsonBox::Value(typeToString(type));
        o["activationType"] = JsonBox::Value(activationTypeToString(activationType));
        JsonBox::Array a;
        for(auto& c:connections){
            a.push_back(c.toJson());
        }
        o["connections"] = JsonBox::Value(a);
        return JsonBox::Value(o);
    }
    std::string Neuron::toString() const noexcept{
        std::ostringstream os;
        os << "output: " << output << ", sum: " << sum << 
              ", delta: " << delta << ", biasWeight: " << biasWeight << ", type: "
              << typeToString(type) << ", activationType: "
              << activationTypeToString(activationType);
        return os.str();
    }
    Neuron& Neuron::addConnection(const Connection& c) noexcept{
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
    void Neuron::clearConnections() noexcept{
        connections.clear();
    }
    bool Neuron::operator==(const Neuron& rhs) const{
        return (output == rhs.output
                && sum == rhs.sum
                && delta == rhs.delta
                && type == rhs.type
                && biasWeight == rhs.biasWeight
                && activationType == rhs.activationType);
    }
    Neuron& Neuron::setBiasWeight(const double& bw) noexcept{
        biasWeight = bw;
        return *this;
    }
    Neuron& Neuron::setActivationType(Neuron::ActivationType atype) noexcept{
        activationType = atype;
        return *this;
    }
    std::string Neuron::typeToString(Neuron::Type t) noexcept{
        // don't set a default so the compiler will tell if we forgot some new case
        switch(t){
            case Neuron::Type::CONTEXT: return "context";
            case Neuron::Type::HIDDEN:  return "hidden";
            case Neuron::Type::INPUT:   return "input";
            case Neuron::Type::OUTPUT:  return "output";
        }
        return "undefined";
    }
    Neuron::Type Neuron::typeToEnum(const std::string& t) noexcept{
        if(t == "input"){
            return Neuron::Type::INPUT;
        }else if(t == "context"){
            return Neuron::Type::CONTEXT;
        }else if(t == "hidden"){
            return Neuron::Type::HIDDEN;
        }else if(t == "output"){
            return Neuron::Type::OUTPUT;
        }
        return Neuron::Type::HIDDEN;
    }
    std::string Neuron::activationTypeToString(Neuron::ActivationType at) noexcept{
        // don't set a default so the compiler will tell if we forgot some new case
        switch(at){
            case Neuron::ActivationType::IDENTITY:          return "identity";
            case Neuron::ActivationType::SIGMOID:           return "sigmoid";
            case Neuron::ActivationType::STEPPED_SIGMOID:   return "steppedSigmoid";
            case Neuron::ActivationType::TANH:              return "tanh";
            case Neuron::ActivationType::SINUSOID:          return "sinusoid";
            case Neuron::ActivationType::RELU:              return "relu";
            case Neuron::ActivationType::NOISY_RELU:        return "noisyRelu";
            case Neuron::ActivationType::LEAKY_RELU:        return "leakyRelu";
            case Neuron::ActivationType::EXPONENTIAL:       return "exponential";
            case Neuron::ActivationType::SOFTMAX:           return "softmax";
            case Neuron::ActivationType::GAUSSIAN:          return "gaussian";
        }
        return "undefined";
    }
    Neuron::ActivationType Neuron::activationTypeToEnum(const std::string& at) noexcept{
        if(at == "identity"){
            return Neuron::ActivationType::IDENTITY;
        }else if(at == "exponential"){
            return Neuron::ActivationType::EXPONENTIAL;
        }else if(at == "leakyRelu"){
            return Neuron::ActivationType::LEAKY_RELU;
        }else if(at == "noisyRelu"){
            return Neuron::ActivationType::NOISY_RELU;
        }else if(at == "relu"){
            return Neuron::ActivationType::RELU;
        }else if(at == "sigmoid"){
            return Neuron::ActivationType::SIGMOID;
        }else if(at == "sinusoid"){
            return Neuron::ActivationType::SINUSOID;
        }else if(at == "steppedSigmoid"){
            return Neuron::ActivationType::STEPPED_SIGMOID;
        }else if(at == "softmax"){
            return Neuron::ActivationType::SOFTMAX;
        }else if(at == "tanh"){
            return Neuron::ActivationType::TANH;
        }else if(at == "gaussian"){
            return Neuron::ActivationType::GAUSSIAN;
        }
        return Neuron::ActivationType::STEPPED_SIGMOID;
    }
}