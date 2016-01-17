#ifndef NNRENDERER_HPP
#define NNRENDERER_HPP

#include <EvoAI/NeuralNetwork.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>

namespace EvoAI{
    /**
     * @class NNRenderer
     * @author Cristian
     * @brief 
     */
    class NNRenderer{
        public:
            NNRenderer(NeuralNetwork* nn);
            void Render(sf::RenderWindow& win,bool renderTexts = false);
            ~NNRenderer() = default;
        private:
            struct NeuronInfo{
                sf::Text output;
                sf::Text sum;
                sf::Text biasWeight;
                sf::Text delta;
                sf::Text type;
                sf::CircleShape neuronShape;
            };
            struct ConnectionInfo{
                sf::Vertex connectionLine[2];
                sf::Text weight;
                sf::Text cycles;
                sf::Text visited;
            };
        private:
            void setUp();
            void setUpNeuronInfo(Neuron& n, sf::Vector2f& p);
            void setUpConnectionInfo(Connection& c, sf::Vector2f& p, const int& space);
        private:
            std::vector<NeuronInfo> neurons;
            std::vector<ConnectionInfo> connections;
            sf::Font font;
            NeuralNetwork* nn;
    };

}

#endif // NNRENDERER_HPP
