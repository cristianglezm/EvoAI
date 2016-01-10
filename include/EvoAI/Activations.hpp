#ifndef ACTIVATIONS_HPP
#define ACTIVATIONS_HPP

#include <cmath>
#include <random>
#include <iostream>

namespace EvoAI{
    class NeuralNetwork;
    struct Activations{
        static double sigmoid(const double& v);
        static double tanh(const double& v);
        static double sinusoid(const double& v);
        static double relu(const double& v);
        static double noisyRelu(const double& v);
        static double leakyRelu(const double& x);
        static double exponential(const double& v);
        static double softmax(const double& v, NeuralNetwork& nn);
    };
    struct Derivatives{
        static double sigmoid(const double& v){
            auto y = Activations::sigmoid(v);
            return (y*(1-y));
        }
        static double tanh(const double& v){
            return 1-std::pow(std::tanh(v),2);
        }
        static double sinusoid(const double& v){
            return std::cos(v);
        }
        static double relu(const double& v){
            return (v > 0.0 ? 1.0:0.0);
        }
    };
}

#endif // ACTIVATIONS_HPP
