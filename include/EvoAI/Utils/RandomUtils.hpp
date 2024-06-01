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
            /**
             * @brief constructor with seed.
             * @param seed 
             */
            RandomGenerator(std::size_t seed);
            /**
             * @brief returns a random number between min and max and multiplies for sqrt of 2.0/layerSize
             * @param min double
             * @param max double
             * @param layerSize double
             * @return double
             */
            double random(double min, double max, double layerSize) noexcept;
            /**
             * @brief LeCun initialization uniform distribution
             * @param inputSize std::size_t
             * @return double
             */
            double lecunInit(std::size_t inputSize) noexcept;
            /**
             * @brief LeCun initialization normal distribution aka Gaussian
             * @param inputSize std::size_t
             * @return double
             */
            double lecunInitNormal(std::size_t inputSize) noexcept;
            /**
             * @brief He initialization uniform distribution
             * @param inputSize std::size_t
             * @return double
             */
            double heInit(std::size_t inputSize) noexcept;
            /**
             * @brief He initialization normal distribution aka Gaussian
             * @param inputSize std::size_t
             * @return double
             */
            double heInitNormal(std::size_t inputSize) noexcept;
            /**
             * @brief Xavier initialization uniform distribution
             * @param inputSize std::size_t
             * @param outputSize std::size_t
             * @return double
             */
            double xavierInit(std::size_t inputSize, std::size_t outputSize) noexcept;
            /**
             * @brief Xavier initialization normal distribution aka Gaussian
             * @param inputSize std::size_t
             * @param outputSize std::size_t
             * @return double
             */
            double xavierInitNormal(std::size_t inputSize, std::size_t outputSize) noexcept;
            /**
             * @brief returns a random number between min and max.
             * @param min T
             * @param max T
             * @return T
             */
            template<typename T>
            T random(T min, T max) noexcept;
            /**
             * @brief gives a random true or false given the rate.
             * @param rate float 0.0-1.0
             * @return bool
             */
            bool random(float rate) noexcept;
            /**
             * @brief get the seed
             * @return std::size_t seed
             */
            inline std::size_t getSeed() const noexcept{
                return m_seed;
            }
            /**
             * @brief set the seed and update engine.
             * @param seed std::size_t
             */
            void setSeed(std::size_t seed) noexcept;
            /**
             * @brief get the engine
             * @return std::mt19937_64 
             */
            inline std::mt19937_64& getEngine() noexcept{
                return m_mtEngine;
            }
            ~RandomGenerator() = default;
        private:
            std::size_t m_seed;
            std::mt19937_64 m_mtEngine;
    };
    template<typename T>
    T RandomGenerator::random(T min, T max) noexcept{
        if constexpr(std::is_floating_point_v<T>){
            std::uniform_real_distribution<T> dice(min, max);
            return dice(m_mtEngine);
        }else if constexpr(std::is_integral_v<T>){
            std::uniform_int_distribution<T> dice(min, max);
            return dice(m_mtEngine);
        }
    }
    /**
     * @brief RandomGenerator
     */
    EvoAI_API RandomGenerator& randomGen() noexcept;
}

#endif // EVOAI_RANDOM_UTILS_HPP
