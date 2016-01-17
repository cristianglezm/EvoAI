#ifndef UTILS_HPP
#define UTILS_HPP

#include <memory>
#include <utility>
#include <random>
#include <chrono>
#include <string>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Audio.hpp>
#include <EvoAI/NeuralNetwork.hpp>

namespace EvoAI{
    /**
     * @brief returns a random number between min and max.
     * @param min
     * @param max
     * @return double
     */
    double random(const double& min, const double& max);
    /**
     * @brief Creates a FeedForward Neural Network.
     * @param numInputs
     * @param numHidden
     * @param numNeuronsPerHidden
     * @param numOutputs
     * @param bias
     * @return 
     */
    std::unique_ptr<NeuralNetwork> createFeedForwardNN(const size_t& numInputs, const size_t& numHidden,const size_t& numNeuronsPerHidden, const std::size_t& numOutputs, const double& bias);
    /**
     * @brief Creates an Elman Neural Network.
     * 
     * This will create a Neural Network with a context layer per every hidden layer.
     * 
     * @param numInputs number of inputs
     * @param numHidden number of hidden layers
     * @param numNeuronsPerHidden number of neurons per hidden layer
     * @param numOutputs number of outputs
     * @param bias bias
     * @return std::unique_ptr<NeuralNetwork>
     */
    std::unique_ptr<NeuralNetwork> createElmanNeuralNetwork(const std::size_t& numInputs, const std::size_t& numHidden, const std::size_t& numNeuronsPerHiddenLayer, const std::size_t& numOutputs, const double& bias);
    /**
     * @brief Generates an Image from coordinates and distance to center
     * 
     * Requires a neural network with 3 inputs and 3 outputs.
     * 
     * @param imageInput filename from the image
     * @param nn std::unique_ptr<NeuralNetwork>
     * @param imageOutput
     */
    void generateImageFromCoordinates(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput);
    /**
     * @brief Generates an Image from coordinates and distance to center
     * 
     * Requires a neural network with 3 inputs and 3 outputs.
     * 
     * @param imageInput image
     * @param nn std::unique_ptr<NeuralNetwork>
     * @param imageOutput
     */
    void generateImageFromCoordinates(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput);
    /**
     * @brief Generates an Image from the color of the imageInput
     * 
     * Requires a neural network with 3 inputs and 3 outputs.
     * 
     * @param imageInput
     * @param nn std::unique_ptr<NeuralNetwork>
     * @param imageOutput
     */
    void generateImageFromColor(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput);
    /**
     * @brief Generates an Image from the color of the imageInput
     * 
     * Requires a neural network with 3 inputs and 3 outputs.
     * 
     * @param imageInput
     * @param nn
     * @param imageOutput
     */
    void generateImageFromColor(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput);
     /**
     * @brief Generates a black And White image from the color of the imageInput.
     * 
     * Requires a Neural network with 3 inputs and 1 output.
     * 
     * @param imageInput std::string
     * @param nn NeuralNetwork
     * @param imageOutput std::string
     */
    void generateBWImageFromColor(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput);
    /**
     * @brief Generates a black And White image from the color of the imageInput.
     * 
     * Requires a Neural network with 3 inputs and 1 output.
     * 
     * @param imageInput
     * @param nn
     * @param imageOutput
     */
    void generateBWImageFromColor(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput);
    /**
     * @brief Generates a Black and White Image from the coords of the imageInput.
     * 
     * Requires a Neural network with 3 inputs and 1 output.
     * 
     * @param imageInput
     * @param nn
     * @param imageOutput
     */
    void generateBWImageFromCoords(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput);
    /**
     * @brief Generates a Black and White Image from the coords of the imageInput.
     * 
     * Requires a Neural network with 3 inputs and 1 output.
     * 
     * @param imageInput
     * @param nn
     * @param imageOutput
     */
    void generateBWImageFromCoords(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput);
    /**
     * @brief Generates a Black And White Image from The Coords of the ImageInput.
     * 
     * Requires a Neural network with 6 inputs and 1 output.
     * 
     * @param imageInput
     * @param nn
     * @param imageOutput
     */
    void generateBWImageFromColorAndCoordinates(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput);
    /**
     * @brief Generates a Black And White Image from The Coords of the ImageInput.
     * 
     * Requires a Neural network with 6 inputs and 1 output.
     * 
     * @param imageInput
     * @param nn
     * @param imageOutput
     */
    void generateBWImageFromColorAndCoordinates(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput);
    /**
     * @brief Generates an Image from the color of the imageInput
     * 
     * Requires a neural network with 6 inputs and 3 outputs.
     * 
     * @param imageInput
     * @param nn std::unique_ptr<NeuralNetwork>
     * @param imageOutput
     */
    void generateImageFromColorAndCoordinates(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput);
    /**
     * @brief Generates an image from the coordinates(x,y) and distance(d) to center and color(r,g,b)
     * 
     * Requires a neural network with 6 inputs and 3 outputs.
     * 
     * @param imageInput
     * @param nn std::unique_ptr<NeuralNetwork>
     * @param imageOutput
     */
    void generateImageFromColorAndCoordinates(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput);
    /**
     * @brief generates a sound from 2d coordinates
     * @param width
     * @param height 
     * @param nn Neural Network
     * @param soundOutput filename of the sound file to save
     */
    void generateSoundFromCoordinates(const int& width, const int& height, NeuralNetwork* nn, const std::string& soundOutput);
    /**
     * @brief 
     * @param Imagefilename
     * @param nn
     * @param soundOutput
     */
    void generateSoundFromColor(const std::string& Imagefilename, NeuralNetwork* nn, const std::string& soundOutput);
    /**
     * @brief 
     * @param imageInput
     * @param nn
     * @param soundOutput
     */
    void generateSoundFromColor(sf::Image& imageInput, NeuralNetwork* nn, const std::string& soundOutput);
}
#endif // UTILS_HPP
