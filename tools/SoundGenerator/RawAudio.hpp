#ifndef EVOAI_RAW_AUDIO_HPP
#define EVOAI_RAW_AUDIO_HPP

#include <memory>
#include <utility>
#include <random>
#include <chrono>
#include <string>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Audio.hpp>

#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Utils.hpp>

namespace EvoAI{
    /**
     * @brief generates a sound from 2d coordinates
     * @param width
     * @param height 
     * @param nn Neural Network
     * @param soundOutput filename of the sound file to save
     */
    void generateSoundFromCoordinates(const int& width, const int& height, NeuralNetwork* nn, const std::string& soundOutput);
    /**
     * @brief generates a sound from the color of a image
     * @param Imagefilename
     * @param nn
     * @param soundOutput
     */
    void generateSoundFromColor(const std::string& Imagefilename, NeuralNetwork* nn, const std::string& soundOutput);
    /**
     * @brief generates a sound from the color of a image.
     * @param imageInput
     * @param nn
     * @param soundOutput
     */
    void generateSoundFromColor(sf::Image& imageInput, NeuralNetwork* nn, const std::string& soundOutput);
    /**
     * @brief generates a sound from the color and coordinates of a image
     * @param Imagefilename
     * @param nn
     * @param soundOutput
     */
    void generateSoundFromColorAndCoordinates(const std::string& Imagefilename, NeuralNetwork* nn, const std::string& soundOutput);
    /**
     * @brief generates a sound from the color and coordinates of a image.
     * @param imageInput
     * @param nn
     * @param soundOutput
     */
    void generateSoundFromColorAndCoordinates(sf::Image& imageInput, NeuralNetwork* nn, const std::string& soundOutput);
}
#endif // EVOAI_RAW_AUDIO_HPP