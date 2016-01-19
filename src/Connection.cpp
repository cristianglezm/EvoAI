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
    , visited(false)
    , gradient(0.0)
    , delta(0.0)
    , oldDelta(0.0){}
    Connection::Connection(const Link& src,const Link& dest,const double& w)
    : src(src)
    , dest(dest)
    , weight(w)
    , cycles(0)
    , visited(false)
    , gradient(0.0)
    , delta(0.0)
    , oldDelta(0.0){}
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
    std::string Connection::toString() const{
        std::ostringstream ss;
        ss << "Recurrent: " << (isRecurrent() ? "True ":"False ")
        << "[" << src.layer << "," << src.neuron << "] --> ["
        << dest.layer << "," << dest.neuron << "]" << " - " << weight << ", grad: " << gradient << ", delta: " << delta;
        return ss.str();
    }
    JsonBox::Value Connection::toJson() const{
        JsonBox::Object o;
        JsonBox::Object JsonSrc;
        JsonSrc["layer"] = JsonBox::Value((int) src.layer);
        JsonSrc["neuron"] = JsonBox::Value((int) src.neuron);
        o["src"] = JsonBox::Value(JsonSrc);
        JsonBox::Object JsonDest;
        JsonDest["layer"] = JsonBox::Value((int) dest.layer);
        JsonDest["neuron"] = JsonBox::Value((int) dest.neuron);
        o["dest"] = JsonBox::Value(JsonDest);
        o["weight"] = weight;
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
    Connection& Connection::setGradient(const double& grad){
        gradient = grad;
        return *this;
    }
    Connection& Connection::addGradient(const double& val){
        gradient += val;
        return *this;
    }
    void Connection::reset(){
        visited = false;
        gradient = 0.0;
        delta = 0.0;
    }
}