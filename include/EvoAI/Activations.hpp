#ifndef EVOAI_ACTIVATIONS_HPP
#define EVOAI_ACTIVATIONS_HPP

#include <cmath>
#include <random>

namespace EvoAI{
    class NeuralNetwork;
    namespace Activations{
        double identity(const double& v);
        /**
         * @brief a sigmoid function
         * @param v
         * @return 
         */
        double sigmoid(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double tanh(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double sinusoid(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double cosine(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double tan(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double relu(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double noisyRelu(const double& v);
        /**
         * @brief 
         * @param x
         * @return 
         */
        double leakyRelu(const double& x);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double exponential(const double& v);
        /**
         * @brief 
         * @param v
         * @param nn
         * @return 
         */
        double softmax(const double& v, NeuralNetwork& nn);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double gaussian(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double modulus(const double& v);
    }
    namespace Derivatives{
        /**
         * @brief 
         * @param v
         * @return 
         */
        double identity(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double sigmoid(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double tanh(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double sinusoid(const double& v);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double relu(const double& v);
        /**
         * @brief 
         * @param v
         * @param nn
         * @return 
         */
        double softmax(const double& v, NeuralNetwork& nn);
        /**
         * @brief 
         * @param v
         * @return 
         */
        double gaussian(const double& v);
    }
}

#endif // EVOAI_ACTIVATIONS_HPP