#ifndef ACTIVATIONS_HPP
#define ACTIVATIONS_HPP

#include <cmath>
#include <iostream>

namespace EvoAI{
    struct Activations{
        static double sigmoid(const double& v){
            return (1.0/(1.0+std::exp(-v)));
        }
        static double tanh(const double& v){
            return std::tanh(v);
        }
        static double sinusoid(const double& v){
            return std::sin(v);
        }
    };
    struct Derivatives{
        static double sigmoid(const double& v){
            return (Activations::sigmoid(v)*(1-Activations::sigmoid(v)));
        }
        static double tanh(const double& v){
            return 1-std::pow(std::tanh(v),2);
        }
        static double sinusoid(const double& v){
            return std::cos(v);
        }
    };
}

#endif // ACTIVATIONS_HPP
