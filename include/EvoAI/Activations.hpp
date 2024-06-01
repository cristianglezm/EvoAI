#ifndef EVOAI_ACTIVATIONS_HPP
#define EVOAI_ACTIVATIONS_HPP

#include <cmath>
#include <numeric>
#include <random>

#include <EvoAI/Export.hpp>
#include <EvoAI/Utils/MathUtils.hpp>

namespace EvoAI{
    class NeuronLayer;
    namespace Activations{
        /**
         *  @brief identity activation
         *  @param v double
         *  @return double
         */
        EvoAI_API double identity(double v) noexcept;
        /**
         * @brief a sigmoid activation
         * @param v double
         * @return double
         */
        EvoAI_API double sigmoid(double v) noexcept;
        /**
         * @brief steepenedSigmoid activation
         * @param v double
         * @return double
         */
        EvoAI_API double steepenedSigmoid(double v) noexcept;
        /**
         * @brief swish activation
         * @param x double
         * @param b double
         * @return double
         */
        EvoAI_API double swish(double x, double b = 1.0) noexcept;
        /**
         * @brief tanh activation
         * @param v double
         * @return double
         */
        EvoAI_API double tanh(double v) noexcept;
        /**
         * @brief sinusoid activation
         * @param v double
         * @return double
         */
        EvoAI_API double sinusoid(double v) noexcept;
        /**
         * @brief cosine activation
         * @param v double
         * @return double
         */
        EvoAI_API double cosine(double v) noexcept;
        /**
         * @brief tan activation
         * @param v double
         * @return double
         */
        EvoAI_API double tan(double v) noexcept;
        /**
         * @brief relu activation
         * @param v double
         * @return double
         */
        EvoAI_API double relu(double v) noexcept;
        /**
         * @brief noisyRelu activation
         * @param v double
         * @return double
         */
        EvoAI_API double noisyRelu(double v) noexcept;
        /**
         * @brief leakyRelu activation
         * @param x double
         * @return double
         */
        EvoAI_API double leakyRelu(double x) noexcept;
        /**
         * @brief exponential activation
         * @param v double
         * @return double
         */
        EvoAI_API double exponential(double v) noexcept;
        /**
         * @brief softmax activation
         * @param outputs NeuronLayer&
         */
        EvoAI_API void softmax(NeuronLayer& outputs) noexcept;
        /**
         * @brief gaussian activation
         * @param v double
         * @return double
         */
        EvoAI_API double gaussian(double v) noexcept;
        /**
         * @brief modulus activation
         * @param v double
         * @param div double
         * @return double
         */
        EvoAI_API double modulus(double v, double div = 2.0) noexcept;
        /**
         * @brief square activation
         * @param v double
         * @return double
         */
        EvoAI_API double square(double v) noexcept;
        /**
         * @brief cube activation
         * @param v double
         * @return double
         */
        EvoAI_API double cube(double v) noexcept;
        /**
         * @brief softplus activation
         * @param v double
         * @return double
         */
        EvoAI_API double softplus(double v) noexcept;
        /**
         * @brief clamp activation
         * @param v double
         * @return double
         */
        EvoAI_API double clamp(double v) noexcept;
        /**
         * @brief inv activation
         * @param v double
         * @return double
         */
        EvoAI_API double inv(double v) noexcept;
        /**
         * @brief log activation
         * @param v double
         * @return double
         */
        EvoAI_API double log(double v) noexcept;
        /**
         * @brief abs activation
         * @param v double
         * @return double
         */
        EvoAI_API double abs(double v) noexcept;
        /**
         * @brief hat activation
         * @param v double
         * @return double
         */
        EvoAI_API double hat(double v) noexcept;
    }
    namespace Derivatives{
        /**
         * @brief identity derivative
         * @param v double
         * @return double
         */
        EvoAI_API double identity(double v) noexcept;
        /**
         * @brief modulus derivative
         * @param v double
         * @param div double
         * @return double
         */
        EvoAI_API double modulus(double v, double div) noexcept;
        /**
         * @brief sigmoid derivative
         * @param v double
         * @return double
         */
        EvoAI_API double sigmoid(double v) noexcept;
        /**
         * @brief a steepened Sigmoid function
         * @param v double
         * @return double
         */
        EvoAI_API double steepenedSigmoid(double v) noexcept;
        /**
         * @brief swish derivative
         * @param x double
         * @param b double
         * @return double
         */
        EvoAI_API double swish(double x, double b = 1.0) noexcept;
        /**
         * @brief tanh derivative
         * @param v double
         * @return double
         */
        EvoAI_API double tanh(double v) noexcept;
        /**
         * @brief cosine derivative
         * @param v double
         * @return double
         */
        EvoAI_API double cosine(double v) noexcept;
        /**
         * @brief tan derivative
         * @param v double
         * @return double
         */
        EvoAI_API double tan(double v) noexcept;
        /**
         * @brief sinusoid derivative
         * @param v double
         * @return double
         */
        EvoAI_API double sinusoid(double v) noexcept;
        /**
         * @brief relu derivative
         * @param v double
         * @return double
         */
        EvoAI_API double relu(double v) noexcept;
        /**
         * @brief noisyRelu derivative
         * @param v double
         * @return double
         */
        EvoAI_API double noisyRelu(double v) noexcept;
        /**
         * @brief leakyRelu derivative
         * @param x double
         * @return double
         */
        EvoAI_API double leakyRelu(double x) noexcept;
        /**
         * @brief exponential derivative
         * @param v double
         * @return double
         */
        EvoAI_API double exponential(double v) noexcept;
        /**
         * @brief Softmax derivative
         * @param outputs NeuronLayer&
         */
        EvoAI_API void softmax(NeuronLayer& outputs) noexcept;
        /**
         * @brief gaussian derivative
         * @param v double
         * @return double
         */
        EvoAI_API double gaussian(double v) noexcept;
        /**
         * @brief square derivative
         * @param v double
         * @return double
         */
        EvoAI_API double square(double v) noexcept;
        /**
         * @brief cube derivative
         * @param v double
         * @return double
         */
        EvoAI_API double cube(double v) noexcept;
        /**
         * @brief softplus derivative
         * @param v double
         * @return double
         */
        EvoAI_API double softplus(double v) noexcept;
        /**
         * @brief clamp derivative
         * @param v double
         * @return double
         */
        EvoAI_API double clamp(double v) noexcept;
        /**
         * @brief inv derivative
         * @param v double
         * @return double
         */
        EvoAI_API double inv(double v) noexcept;
        /**
         * @brief log derivative
         * @param v double
         * @return double
         */
        EvoAI_API double log(double v) noexcept;
        /**
         * @brief abs derivative
         * @param v double
         * @return double
         */
        EvoAI_API double abs(double v) noexcept;
        /**
         * @brief hat derivative
         * @param v double
         * @return double
         */
        EvoAI_API double hat(double v) noexcept;
    }
}

#endif // EVOAI_ACTIVATIONS_HPP
