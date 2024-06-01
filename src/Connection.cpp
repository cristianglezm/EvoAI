#include <EvoAI/Connection.hpp>
#include <EvoAI/Utils/MathUtils.hpp>
#include <EvoAI/Utils/RandomUtils.hpp>

namespace EvoAI{
    Link::Link(std::size_t lyr, std::size_t nrn)
    : layer(lyr)
    , neuron(nrn){}
    Link::Link(JsonBox::Object o)
    : layer(std::stoull(o["layer"].tryGetString("0")))
    , neuron(std::stoull(o["neuron"].tryGetString("0"))){
        if(layer == std::numeric_limits<std::size_t>::max()){
            layer = 0;
        }
        if(neuron == std::numeric_limits<std::size_t>::max()){
            neuron = 0;
        }
    }
    JsonBox::Value Link::toJson() const noexcept{
        JsonBox::Object o;
        o["layer"] = JsonBox::Value(std::to_string(layer));
        o["neuron"] = JsonBox::Value(std::to_string(neuron));
        return JsonBox::Value(o);
    }
    bool Link::operator<(const Link& rhs) const{
        return (layer < rhs.layer);
    }
    bool Link::operator==(const Link& rhs) const{
        return (layer == rhs.layer
                && neuron == rhs.neuron);
    }
    Connection::Connection()
    : src(Link(0,0))
    , dest(Link(0,0))
    , weight(0.0)
    , gradient(0.0)
    , cycles(0)
    , visited(false)
    , frozen(false){}
    Connection::Connection(const Link& source, const Link& destination)
    : src(source)
    , dest(destination)
    , weight(0.0)
    , gradient(0.0)
    , cycles(0)
    , visited(false)
    , frozen(false){}
    Connection::Connection(const Link& source, const Link& destination, double w)
    : src(source)
    , dest(destination)
    , weight(w)
    , gradient(0.0)
    , cycles(0)
    , visited(false)
    , frozen(false){}
    Connection::Connection(JsonBox::Object o)
    : src(o["src"].getObject())
    , dest(o["dest"].getObject())
    , weight(o["weight"].tryGetDouble(0.0))
    , gradient(o["gradient"].tryGetDouble(0.0))
    , cycles(0)
    , visited(false)
    , frozen(o["frozen"].tryGetBoolean(false)){}
    Connection& Connection::setSrc(const Link& l){
        src = l;
        return *this;
    }
    Connection& Connection::setDest(const Link& l){
        dest = l;
        return *this;
    }
    Connection& Connection::setVisited(bool b){
        visited = b;
        return *this;
    }
    Connection& Connection::setWeight(double w){
        if(!frozen){
            weight = w;
        }
        return *this;
    }
    Connection& Connection::setCycles(int c){
        cycles = c;
        return *this;
    }
    std::string Connection::toString() const noexcept{
        std::ostringstream ss;
        ss << "Recurrent: " << (isRecurrent() ? "True ":"False ")
            << "[" << src.layer << "," << src.neuron << "] --> ["
            << dest.layer << "," << dest.neuron << "]" << " - weight: " << weight << ", grad: " << gradient << ", frozen: " << frozen;
        return ss.str();
    }
    JsonBox::Value Connection::toJson() const noexcept{
        JsonBox::Object o;
        o["src"] = src.toJson();
        o["dest"] = dest.toJson();
        o["weight"] = weight;
        o["gradient"] = gradient;
        o["frozen"] = frozen;
        return JsonBox::Value(o);
    }
    bool Connection::operator==(const Connection& rhs) const{
        return (src == rhs.src
                && dest == rhs.dest
                && weight == rhs.weight
                && cycles == rhs.cycles
                && visited == rhs.visited
                && gradient == rhs.gradient);
    }
    Connection& Connection::setGradient(double grad) noexcept{
        gradient = grad;
        return *this;
    }
    Connection& Connection::addGradient(double val) noexcept{
        gradient += val;
        return *this;
    }
    void Connection::reset(){
        visited = false;
        gradient = 0.0;
    }
    Connection& Connection::setFrozen(bool frzen) noexcept{
        frozen = frzen;
        return *this;
    }
}