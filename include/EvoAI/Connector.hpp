#ifndef EVOAI_CONNECTOR_HPP
#define EVOAI_CONNECTOR_HPP

#include <vector>

#include <EVoAI/NeuralNetwork.hpp>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @brief This class Will connect NeuralNetworks between them.
     * @todo implement
     */
    class EvoAI_API Connector final{
        public:
            Connector(){
                
            }

            ~Connector() = default;
        private:
            std::vector<NeuralNetwork*> nns;
    };
}
#endif // EVOAI_CONNECTOR_HPP