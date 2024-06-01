#ifndef EVOAI_NNRENDERER_HPP
#define EVOAI_NNRENDERER_HPP

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
             * @param NN NeuralNetwork*
             */
            NNRenderer(NeuralNetwork* NN);
            /**
             * @brief it will renderize the neural network
             * @param win window
             * @param renderTexts to render info or not
             */
            void Render(sf::RenderWindow& win, bool renderTexts = false) noexcept;
            /**
             * @brief Changes the neural network, is the same as building a new NNRenderer.
             * @param nn NeuralNetwork*
             */
            void setNeuralNetwork(NeuralNetwork* NN) noexcept;
            /**
             * @brief sets the Font to use
             * The font is copied so you can throw it away.
             * @param f sf::Font
             */
            void setFont(sf::Font f) noexcept;
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
             * @brief sets the info of the neural network
             */
            void setUp() noexcept;
            /**
             * @brief Parses the info of the neuron
             * @param n Neuron
             * @param p Position
             */
            void setUpNeuronInfo(Neuron& n, sf::Vector2f& p) noexcept;
            /**
             * @brief Parses the info of the Connection
             * @param c Connection
             * @param p Position
             * @param space space for the weights
             */
            void setUpConnectionInfo(Connection& c, sf::Vector2f& p, int space) noexcept;
        private:
            std::vector<NeuronInfo> neurons;
            std::vector<ConnectionInfo> connections;
            sf::Font font;
            NeuralNetwork* nn;
    };

}

#endif // EVOAI_NNRENDERER_HPP