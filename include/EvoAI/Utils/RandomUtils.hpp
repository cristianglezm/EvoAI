#ifndef EVOAI_RANDOM_UTILS_HPP
#define EVOAI_RANDOM_UTILS_HPP

#include <memory>
#include <utility>
#include <random>
#include <chrono>
#include <string>

#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @brief Class to generate random numbers
     */
    class EvoAI_API RandomGenerator final{
        public:
            RandomGenerator();
            RandomGenerator(const unsigned& seed);
            /**
             * @brief returns a random number between min and max and multiplies for sqrt of 2.0/layerSize
             * @param min const double&
             * @param max const double&
             * @param layerSize const double&
             * @return double
             */
            double random(const double& min, const double& max, const double& layerSize) noexcept;
            /**
             * @brief returns a random number between min and max.
             * @param min const double&
             * @param max const double&
             * @return double
             */
            double random(const double& min, const double& max) noexcept;
            /**
             * @brief returns a random number between min and max.
             * @param min const int&
             * @param max const int&
             * @return int
             */
            int random(const int& min, const int& max) noexcept;
            /**
             * @brief returns a random number between min and max.
             * @param min const unsigned int&
             * @param max const unsigned int&
             * @return unsigned int
             */
            unsigned int random(const unsigned int& min, const unsigned int& max) noexcept;
            /**
             * @brief gives a random true or false given the rate.
             * @param rate const float& 0.0-1.0
             * @return bool
             */
            bool random(const float& rate) noexcept;
            ~RandomGenerator() = default;
        private:
            unsigned seed;
            std::mt19937 mtEngine;
            
    };
    /**
     * @brief static RandomGenerator
     */
    static RandomGenerator randomGen = RandomGenerator();
}

#endif // EVOAI_RANDOM_UTILS_HPP