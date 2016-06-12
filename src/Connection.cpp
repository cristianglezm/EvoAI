#include <EvoAI/Connection.hpp>

namespace EvoAI{
    Link::Link(const std::size_t& layer,const std::size_t& neuron)
    : layer(layer)
    , neuron(neuron){}
    Link::Link(JsonBox::Object o)
    : layer(std::stoull(o["layer"].getString()))
    , neuron(std::stoull(o["neuron"].getString())){}
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
    , cycles(0)
    , visited(false)
    , gradient(0.0)
    , delta(0.0)
    , oldDelta(0.0){}
    Connection::Connection(const Link& src, const Link& dest)
    : src(src)
    , dest(dest)
    , weight(0.0)
    , cycles(0)
    , visited(false)
    , gradient(0.0)
    , delta(0.0)
    , oldDelta(0.0){}
    Connection::Connection(const Link& src, const Link& dest, const double& w)
    : src(src)
    , dest(dest)
    , weight(w)
    , cycles(0)
    , visited(false)
    , gradient(0.0)
    , delta(0.0)
    , oldDelta(0.0){}
    Connection::Connection(JsonBox::Object o)
    : src(o["src"].getObject())
    , dest(o["dest"].getObject())
    , weight(o["weight"].getDouble())
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
    std::string Connection::toString() const noexcept{
        std::ostringstream ss;
        ss << "Recurrent: " << (isRecurrent() ? "True ":"False ")
        << "[" << src.layer << "," << src.neuron << "] --> ["
        << dest.layer << "," << dest.neuron << "]" << " - " << weight << ", grad: " << gradient << ", delta: " << delta;
        return ss.str();
    }
    JsonBox::Value Connection::toJson() const noexcept{
        JsonBox::Object o;
        o["src"] = src.toJson();
        o["dest"] = dest.toJson();
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
    Connection& Connection::setGradient(const double& grad) noexcept{
        gradient = grad;
        return *this;
    }
    Connection& Connection::addGradient(const double& val) noexcept{
        gradient += val;
        return *this;
    }
    Connection& Connection::setDelta(const double& val) noexcept{
        oldDelta = delta;
        delta = val;
        return *this;
    }
    void Connection::reset(){
        visited = false;
        gradient = 0.0;
        delta = 0.0;
    }
}