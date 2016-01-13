#ifndef ACTIVATIONS_HPP
#define ACTIVATIONS_HPP

#include <cmath>
#include <random>
#include <iostream>

namespace EvoAI{
    class NeuralNetwork;
    namespace Activations{
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
    };
    namespace Derivatives{
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
         * @return 
         */
        double gaussian(const double& v);
    };
}

#endif // ACTIVATIONS_HPP
