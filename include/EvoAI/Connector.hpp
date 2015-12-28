#ifndef CONNECTOR_HPP

#include <vector>
#include <EVoAI/NeuralNetwork.hpp>

namespace EvoAI{
    /**
     * @brief This class Will connect NeuralNetworks between them.
     */
    class Connector final{
        public:
            Connector(){
                
            }

            ~Connector() = default;
        private:
            std::vector<NeuralNetwork*> nns;
    };
}
#endif // CONNECTOR_HPP
