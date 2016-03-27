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
            /**
             * @brief default Constructor
             * @param nn NeuralNetwork*
             */
            NNRenderer(NeuralNetwork* nn);
            /**
             * @brief it will renderize the neural network
             * @param win window
             * @param renderTexts to render info or not
             */
            void Render(sf::RenderWindow& win,bool renderTexts = false);
            ~NNRenderer() = default;
        private:
            /**
             * @class NeuronInfo
             * @author Cristian Glez <cristian.glez.m@gmail.com>
             * @brief simple private struct to store info for the neuron
             */
            struct NeuronInfo{
                sf::Text info;
                sf::CircleShape neuronShape;
            };
            /**
             * @class ConnectionInfo
             * @author Cristian Glez <cristian.glez.m@gmail.com>
             * @brief simple private struct to store info for the connection.
             */
            struct ConnectionInfo{
                sf::Vertex connectionLine[2];
                sf::Text info;
            };
        private:
            /**
             * @brief 
             */
            void setUp();
            /**
             * @brief 
             * @param n
             * @param p
             */
            void setUpNeuronInfo(Neuron& n, sf::Vector2f& p);
            /**
             * @brief 
             * @param c
             * @param p
             */
            void setUpConnectionInfo(Connection& c, sf::Vector2f& p, const int& space);
        private:
            std::vector<NeuronInfo> neurons;
            std::vector<ConnectionInfo> connections;
            sf::Font font;
            NeuralNetwork* nn;
    };

}

#endif // NNRENDERER_HPP
