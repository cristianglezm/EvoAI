#ifndef EVOAI_ACTIVATIONS_HPP
#define EVOAI_ACTIVATIONS_HPP

#include <cmath>
#include <random>

#include <EvoAI/Export.hpp>

namespace EvoAI{
    class NeuralNetwork;
    /**
     * @todo review
     * @todo add [add, mult, div?, abs, ?]
     */
    namespace Activations{
        EvoAI_API double identity(const double& v);
        /**
         * @brief a sigmoid function
         * @param v
         * @return 
         */
        EvoAI_API double sigmoid(const double& v);
        /**
         * @brief a steepened Sigmoid function
         * @param v
         * @return 
         */
        EvoAI_API double steepenedSigmoid(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double tanh(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double sinusoid(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double cosine(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double tan(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double relu(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double noisyRelu(const double& v);
        /**
         * @brief 
         * @param x
         * @return 
         */
        EvoAI_API double leakyRelu(const double& x);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double exponential(const double& v);
        /**
         * @brief 
         * @param v
         * @param nn
         * @return 
         */
        EvoAI_API double softmax(const double& v, NeuralNetwork& nn);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double gaussian(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double modulus(const double& v);
        /**
         * @brief
         * @param v
         * @return
         */
        EvoAI_API double square(const double& v);
        /**
         * @brief
         * @param v
         * @return
         */
        EvoAI_API double cube(const double& v);
    }
    namespace Derivatives{
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double identity(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double sigmoid(const double& v);
        /**
         * @brief a steepened Sigmoid function
         * @param v
         * @return 
         */
        EvoAI_API double steepenedSigmoid(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double tanh(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double cosine(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double tan(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double sinusoid(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double relu(const double& v);
        /**
         * @brief 
         * @param v
         * @param nn
         * @return 
         */
        EvoAI_API double softmax(const double& v, NeuralNetwork& nn);
        /**
         * @brief 
         * @param v
         * @return 
         */
        EvoAI_API double gaussian(const double& v);
        /**
         * @brief
         * @param v
         * @return
         */
        EvoAI_API double square(const double& v);
        /**
         * @brief
         * @param v
         * @return
         */
        EvoAI_API double cube(const double& v);
    }
}

#endif // EVOAI_ACTIVATIONS_HPP