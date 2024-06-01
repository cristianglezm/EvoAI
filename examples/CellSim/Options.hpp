#ifndef EVOAI_OPTIONS_HPP
#define EVOAI_OPTIONS_HPP

#include <array>
#include <string>
#include <chrono>
#include <JsonBox.h>

#include <SFML/Graphics/Rect.hpp>

namespace EvoAI{
    /**
     * @brief Options for CellSim
     */
    struct Options final{
        Options() noexcept;
        Options(JsonBox::Object o) noexcept;
        JsonBox::Value toJson() const noexcept;
        // data
        std::array<float, 3> coefficients;
        std::string fontFilename;
        std::string saveFilename;
        std::string loadFilename;
        sf::FloatRect screen;
        std::size_t maxCellNum;
        std::size_t rounds;
        std::size_t seed;
        std::size_t numSamples;
        std::size_t epoch;
        std::size_t batchSize;
        double learningRate;
        double compatibilityThreshold;
        std::size_t speciesMaxAge;
        std::chrono::seconds secondsForNextGen;
        bool interspecies;
        bool renderInfo;
        bool pause;
        bool help;
        bool ignoreTimeCounter;
        bool renderSensorPoints;
        bool renderBrainActivity;
        bool writeAverages;
        bool writeBestBrain;
        bool training;
    };
    Options parseOptions(int argc, const char** argv);
}

#endif // EVOAI_OPTIONS_HPP