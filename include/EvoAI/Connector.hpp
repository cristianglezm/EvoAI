#ifndef CONNECTOR_HPP

namespace EvoAI{
    /**
     * @brief This class Will connect NeuralNetworks between them.
     */
    class Connector final{
        public:
            Connector();

            ~Connector();
        private:
            std::vector<NeuralNetwork*> nns;
    };
}
#endif // CONNECTOR_HPP
