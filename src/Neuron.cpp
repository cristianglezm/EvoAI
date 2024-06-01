#include <EvoAI/Neuron.hpp>
#include <EvoAI/Utils.hpp>

namespace EvoAI{
    Neuron::Neuron()
    : output(0.0)
    , sum(0.0)
    , gradient(0.0)
    , biasWeight(Link(0,0), Link(0,0), randomGen().random(-1.0,1.0))
    , type(Type::HIDDEN)
    , activationType(Neuron::ActivationType::SIGMOID)
    , connections(){}
    Neuron::Neuron(JsonBox::Object o)
    : output(0.0)
    , sum(0.0)
    , gradient(o["gradient"].tryGetDouble(0.0))
    , biasWeight(Connection(o["biasWeight"].getObject()))
    , type(Neuron::typeToEnum(o["type"].getString()))
    , activationType(Neuron::activationTypeToEnum(o["activationType"].getString()))
    , connections(){
        auto& conns = o["connections"].getArray();
        connections.reserve(conns.size());
        for(auto& c:conns){
            connections.emplace_back(c.getObject());
        }
    }
    Neuron::Neuron(Type t)
    : output(0.0)
    , sum(0.0)
    , gradient(0.0)
    , biasWeight(Link(0,0), Link(0,0), randomGen().random(-1.0,1.0))
    , type(t)
    , activationType(Neuron::ActivationType::SIGMOID)
    , connections(){}
    Neuron& Neuron::setType(Type t) noexcept{
        type = t;
        return *this;
    }
    Neuron& Neuron::setOutput(double out) noexcept{
        output = out;
        return *this;
    }
    Neuron& Neuron::addSum(double val) noexcept{
        sum += val;
        return *this;
    }
    Neuron& Neuron::setSum(double val) noexcept{
        this->sum = val;
        return *this;
    }
    Neuron& Neuron::setGradient(double grad) noexcept{
        this->gradient = grad;
        return *this;
    }
    Neuron& Neuron::addGradient(double grad) noexcept{
        this->gradient += grad;
        return *this;
    }
    Neuron& Neuron::reset() noexcept{
       if(type != Type::CONTEXT){
            output = 0.0;
            sum = 0.0;
            gradient = 0.0;
       }
       return *this;
    }
    Neuron& Neuron::resetContext() noexcept{
        output = 0.0;
        sum = 0.0;
        gradient = 0.0;
        return *this;
    }
    JsonBox::Value Neuron::toJson() const noexcept{
        JsonBox::Object o;
        o["biasWeight"] = biasWeight.toJson();
        o["gradient"] = gradient;
        o["type"] = JsonBox::Value(typeToString(type));
        o["activationType"] = JsonBox::Value(activationTypeToString(activationType));
        JsonBox::Array conns;
        conns.reserve(connections.size());
        for(auto& c:connections){
            conns.emplace_back(c.toJson());
        }
        o["connections"] = JsonBox::Value(conns);
        return JsonBox::Value(o);
    }
    std::string Neuron::toString(const std::string& delimiter) const noexcept{
        std::ostringstream os;
        os << "output: " << output << delimiter << " sum: " << sum << 
              delimiter << " gradient: " << gradient << delimiter << " biasWeight: " << biasWeight.getWeight() << delimiter << " biasGrad: " 
              << biasWeight.getGradient() << delimiter << " type: " <<
              typeToString(type) << delimiter << " activationType: " << activationTypeToString(activationType);
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
    std::size_t Neuron::size(){
        return connections.size();
    }
    Connection& Neuron::operator[](std::size_t index){
        return connections[index];
    }
    bool Neuron::operator==(const Neuron& rhs) const{
        return (output == rhs.output
                && sum == rhs.sum
                && gradient == rhs.gradient
                && type == rhs.type
                && biasWeight == rhs.biasWeight
                && activationType == rhs.activationType);
    }
    Neuron& Neuron::setBiasWeight(double bw) noexcept{
        biasWeight.setWeight(bw);
        return *this;
    }
    Neuron& Neuron::setBiasGradient(double grad) noexcept{
        biasWeight.setGradient(grad);
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
            case Neuron::ActivationType::MODULUS:           return "modulus";
            case Neuron::ActivationType::SIGMOID:           return "sigmoid";
            case Neuron::ActivationType::STEPPED_SIGMOID:   return "steppedSigmoid";
            case Neuron::ActivationType::SWISH:             return "swish";
            case Neuron::ActivationType::TANH:              return "tanh";
            case Neuron::ActivationType::TAN:               return "tan";
            case Neuron::ActivationType::SINUSOID:          return "sinusoid";
            case Neuron::ActivationType::COSINE:            return "cosine";
            case Neuron::ActivationType::SQUARE:            return "square";
            case Neuron::ActivationType::CUBE:              return "cube";
            case Neuron::ActivationType::RELU:              return "relu";
            case Neuron::ActivationType::NOISY_RELU:        return "noisyRelu";
            case Neuron::ActivationType::LEAKY_RELU:        return "leakyRelu";
            case Neuron::ActivationType::EXPONENTIAL:       return "exponential";
            case Neuron::ActivationType::SOFTMAX:           return "softmax";
            case Neuron::ActivationType::GAUSSIAN:          return "gaussian";
            case Neuron::ActivationType::SOFTPLUS:          return "softplus";
            case Neuron::ActivationType::CLAMP:             return "clamp";
            case Neuron::ActivationType::INV:               return "inv";
            case Neuron::ActivationType::LOG:               return "log";
            case Neuron::ActivationType::ABS:               return "abs";
            case Neuron::ActivationType::HAT:               return "hat";
        }
        return "undefined";
    }
    Neuron::ActivationType Neuron::activationTypeToEnum(const std::string& at) noexcept{
        if(at == "identity"){
            return Neuron::ActivationType::IDENTITY;
        }else if(at == "modulus"){
            return Neuron::ActivationType::MODULUS;
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
        }else if(at == "square"){
            return Neuron::ActivationType::SQUARE;
        }else if(at == "cube"){
            return Neuron::ActivationType::CUBE;
        }else if(at == "steppedSigmoid"){
            return Neuron::ActivationType::STEPPED_SIGMOID;
        }else if(at == "swish"){
            return Neuron::ActivationType::SWISH;
        }else if(at == "softmax"){
            return Neuron::ActivationType::SOFTMAX;
        }else if(at == "tanh"){
            return Neuron::ActivationType::TANH;
        }else if(at == "gaussian"){
            return Neuron::ActivationType::GAUSSIAN;
        }else if(at == "cosine"){
            return Neuron::ActivationType::COSINE;
        }else if(at == "tan"){
            return Neuron::ActivationType::TAN;
        }else if(at == "softplus"){
            return Neuron::ActivationType::SOFTPLUS;
        }else if(at == "clamp"){
            return Neuron::ActivationType::CLAMP;
        }else if(at == "inv"){
            return Neuron::ActivationType::INV;
        }else if(at == "log"){
            return Neuron::ActivationType::LOG;
        }else if(at == "abs"){
            return Neuron::ActivationType::ABS;
        }else if(at == "hat"){
            return Neuron::ActivationType::HAT;
        }
        return Neuron::ActivationType::SIGMOID;
    }
}
