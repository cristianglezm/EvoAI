#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <sstream>
#include <JsonBox.h>

namespace EvoAI{
    struct Link{
        Link(const std::size_t& layer,const std::size_t& neuron);
        bool operator<(const Link& rhs) const;
        bool operator==(const Link& rhs) const;
        std::size_t layer;
        std::size_t neuron;
    };
    class Connection{
        public:
            Connection(const Link& src,const Link& dest);
            Connection(const Link& src,const Link& dest,const double& w);
            inline bool isRecurrent() const{ return !(src<dest); }
            Connection& setSrc(const Link& l);
            inline const Link& getSrc() const{ return src; }
            Connection& setDest(const Link& l);
            inline const Link& getDest() const{ return dest; }
            inline const bool& isVisited(){ return visited; }
            Connection& setVisited(bool b);
            inline const double& getWeight(){ return weight; }
            Connection& setWeight(const double& w);
            Connection& setCycles(const int& c);
            inline const int& getCycles(){ return cycles; }
            std::string toString();
            JsonBox::Value toJson();
            bool operator==(const Connection& rhs) const;
        private:
            Link src;
            Link dest;
            double weight;
            int cycles;
            bool visited;
    };
}
#endif // CONNECTION_HPP
