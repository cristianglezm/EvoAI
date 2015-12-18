#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <sstream>

namespace EvoAI{
    struct Link{
        Link(const std::size_t& layer,const std::size_t& neuron)
        : layer(layer)
        , neuron(neuron){}
        std::size_t layer;
        std::size_t neuron;
        bool operator<(const Link& rhs) const{
            return (layer < rhs.layer);
        }
        bool operator==(const Link& rhs) const{
            return (layer == rhs.layer
                    && neuron == rhs.neuron);
        }
    };
    class Connection{
        public:
            Connection(const Link& src,const Link& dest)
            : src(src)
            , dest(dest)
            , weight(0.0)
            , visited(false){}
            Connection(const Link& src,const Link& dest,const double& w)
            : src(src)
            , dest(dest)
            , weight(w)
            , visited(false){}
            bool isRecurrent() const{
                return !(src<dest);
            }
            Connection& setSrc(const Link& l){
                src = l;
                return *this;
            }
            const Link& getSrc() const{
                return src;
            }
            Connection& setDest(const Link& l){
                dest = l;
                return *this;
            }
            const Link& getDest() const{
                return dest;
            }
            const bool& isVisited(){
                return visited;
            }
            Connection& setVisited(bool b){
                visited = b;
                return *this;
            }
            const double& getWeight(){
                return weight;
            }
            Connection& setWeight(const double& w){
                weight = w;
                return *this;
            }
            Connection& setCycles(const int& c){
                cycles = c;
                return *this;
            }
            const int& getCycles(){ return cycles; }
            std::string toString(){
                std::ostringstream ss;
                ss << "Recurrent: " << (isRecurrent() ? "True ":"False ")
                << "[" << src.layer << "," << src.neuron << "] --> ["
                << dest.layer << "," << dest.neuron << "]" << " - " << getWeight();
                return ss.str();
            }
            bool operator==(const Connection& rhs) const{
                return (src == rhs.src
                        && dest == rhs.dest
                        && weight == rhs.weight
                        && cycles == rhs.cycles
                        && visited == rhs.visited);
            }
        private:
            Link src;
            Link dest;
            double weight;
            int cycles;
            bool visited;
    };
}
#endif // CONNECTION_HPP
