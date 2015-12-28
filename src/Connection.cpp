#include <EvoAI/Connection.hpp>

namespace EvoAI{
    Link::Link(const std::size_t& layer,const std::size_t& neuron)
    : layer(layer)
    , neuron(neuron){}
    bool Link::operator<(const Link& rhs) const{
        return (layer < rhs.layer);
    }
    bool Link::operator==(const Link& rhs) const{
        return (layer == rhs.layer
                && neuron == rhs.neuron);
    }
    Connection::Connection(const Link& src,const Link& dest)
    : src(src)
    , dest(dest)
    , weight(0.0)
    , cycles(0)
    , visited(false){}
    Connection::Connection(const Link& src,const Link& dest,const double& w)
    : src(src)
    , dest(dest)
    , weight(w)
    , cycles(0)
    , visited(false){}
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
    Connection& Connection::setWeight(const double& w){
        weight = w;
        return *this;
    }
    Connection& Connection::setCycles(const int& c){
        cycles = c;
        return *this;
    }
    std::string Connection::toString(){
        std::ostringstream ss;
        ss << "Recurrent: " << (isRecurrent() ? "True ":"False ")
        << "[" << src.layer << "," << src.neuron << "] --> ["
        << dest.layer << "," << dest.neuron << "]" << " - " << getWeight();
        return ss.str();
    }
    JsonBox::Value Connection::toJson(){
        using namespace JsonBox;
        Object o;
        Object JsonSrc;
        JsonSrc["layer"] = Value((int) src.layer);
        JsonSrc["neuron"] = Value((int) src.neuron);
        o["src"] = Value(JsonSrc);
        Object JsonDest;
        JsonDest["layer"] = Value((int) dest.layer);
        JsonDest["neuron"] = Value((int) dest.neuron);
        o["dest"] = Value(JsonDest);
        o["weight"] = weight;
        o["cycles"] = cycles;
        o["visited"] = visited;
        return Value(o);
    }
    bool Connection::operator==(const Connection& rhs) const{
        return (src == rhs.src
                && dest == rhs.dest
                && weight == rhs.weight
                && cycles == rhs.cycles
                && visited == rhs.visited);
    }
}