#ifndef EVOAI_IMAGEUTILS_HPP
#define EVOAI_IMAGEUTILS_HPP

#include <memory>
#include <utility>
#include <random>
#include <chrono>
#include <string>

#include <SFML/Graphics/Image.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Utils.hpp>

namespace EvoAI{
    namespace imageUtils{
        /**
         * @brief Generates an Image from coordinates and distance to center
         * 
         * Requires a neural network with 3 inputs and 3 outputs.
         * 
         * @param imageInput filename from the image
         * @param nn std::unique_ptr<NeuralNetwork>
         * @param imageOutput
         */
        void generateImageFromCoordinates(const std::string& imageInput, NeuralNetwork::NeuralNetwork* nn, const std::string& imageOutput);
        /**
         * @brief Generates an Image from coordinates and distance to center
         * 
         * Requires a neural network with 3 inputs and 3 outputs.
         * 
         * @param imageInput image
         * @param nn std::unique_ptr<NeuralNetwork>
         * @param imageOutput
         */
        void generateImageFromCoordinates(sf::Image& imageInput, NeuralNetwork::NeuralNetwork* nn, const std::string& imageOutput);
        /**
         * @brief Generates an Image from the color of the imageInput
         * 
         * Requires a neural network with 3 inputs and 3 outputs.
         * 
         * @param imageInput
         * @param nn std::unique_ptr<NeuralNetwork>
         * @param imageOutput
         */
        void generateImageFromColor(const std::string& imageInput, NeuralNetwork::NeuralNetwork* nn, const std::string& imageOutput);
        /**
         * @brief Generates an Image from the color of the imageInput
         * 
         * Requires a neural network with 3 inputs and 3 outputs.
         * 
         * @param imageInput
         * @param nn
         * @param imageOutput
         */
        void generateImageFromColor(sf::Image& imageInput, NeuralNetwork::NeuralNetwork* nn, const std::string& imageOutput);
         /**
         * @brief Generates a black And White image from the color of the imageInput.
         * 
         * Requires a Neural network with 3 inputs and 1 output.
         * 
         * @param imageInput std::string
         * @param nn NeuralNetwork
         * @param imageOutput std::string
         */
        void generateBWImageFromColor(const std::string& imageInput, NeuralNetwork::NeuralNetwork* nn, const std::string& imageOutput);
        /**
         * @brief Generates a black And White image from the color of the imageInput.
         * 
         * Requires a Neural network with 3 inputs and 1 output.
         * 
         * @param imageInput
         * @param nn
         * @param imageOutput
         */
        void generateBWImageFromColor(sf::Image& imageInput, NeuralNetwork::NeuralNetwork* nn, const std::string& imageOutput);
        /**
         * @brief Generates a Black and White Image from the coords of the imageInput.
         * 
         * Requires a Neural network with 3 inputs and 1 output.
         * 
         * @param imageInput
         * @param nn
         * @param imageOutput
         */
        void generateBWImageFromCoords(const std::string& imageInput, NeuralNetwork::NeuralNetwork* nn, const std::string& imageOutput);
        /**
         * @brief Generates a Black and White Image from the coords of the imageInput.
         * 
         * Requires a Neural network with 3 inputs and 1 output.
         * 
         * @param imageInput
         * @param nn
         * @param imageOutput
         */
        void generateBWImageFromCoords(sf::Image& imageInput, NeuralNetwork::NeuralNetwork* nn, const std::string& imageOutput);
        /**
         * @brief Generates a Black And White Image from The Coords of the ImageInput.
         * 
         * Requires a Neural network with 6 inputs and 1 output.
         * 
         * @param imageInput
         * @param nn
         * @param imageOutput
         */
        void generateBWImageFromColorAndCoordinates(const std::string& imageInput, NeuralNetwork::NeuralNetwork* nn, const std::string& imageOutput);
        /**
         * @brief Generates a Black And White Image from The Coords of the ImageInput.
         * 
         * Requires a Neural network with 6 inputs and 1 output.
         * 
         * @param imageInput
         * @param nn
         * @param imageOutput
         */
        void generateBWImageFromColorAndCoordinates(sf::Image& imageInput, NeuralNetwork::NeuralNetwork* nn, const std::string& imageOutput);
        /**
         * @brief Generates an Image from the color of the imageInput
         * 
         * Requires a neural network with 6 inputs and 3 outputs.
         * 
         * @param imageInput
         * @param nn std::unique_ptr<NeuralNetwork>
         * @param imageOutput
         */
        void generateImageFromColorAndCoordinates(const std::string& imageInput, NeuralNetwork::NeuralNetwork* nn, const std::string& imageOutput);
        /**
         * @brief Generates an image from the coordinates(x,y) and distance(d) to center and color(r,g,b)
         * 
         * Requires a neural network with 6 inputs and 3 outputs.
         * 
         * @param imageInput
         * @param nn std::unique_ptr<NeuralNetwork>
         * @param imageOutput
         */
        void generateImageFromColorAndCoordinates(sf::Image& imageInput, NeuralNetwork::NeuralNetwork* nn, const std::string& imageOutput);
    }
}
#endif // EVOAI_IMAGEUTILS_HPP