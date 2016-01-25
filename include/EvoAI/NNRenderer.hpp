#ifndef NNRENDERER_HPP
#define NNRENDERER_HPP

#include <EvoAI/NeuralNetwork.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>

namespace EvoAI{
    /**
     * @class NNRenderer
     * @author Cristian Glez <cristian.glez.m@gmail.com>
     * @brief Class for drawing Neural Networks
     */
    class NNRenderer{
        public:
            NNRenderer(NeuralNetwork* nn);
            void Render(sf::RenderWindow& win,bool renderTexts = false);
            ~NNRenderer() = default;
        private:
            struct NeuronInfo{
                sf::Text info;
                sf::CircleShape neuronShape;
            };
            struct ConnectionInfo{
                sf::Vertex connectionLine[2];
                sf::Text info;
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
