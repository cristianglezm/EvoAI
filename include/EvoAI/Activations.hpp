#ifndef ACTIVATIONS_HPP
#define ACTIVATIONS_HPP

#include <cmath>
#include <random>
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
        static double relu(const double& v){
            return std::max(0.0,v);
        }
        static double noisyRelu(const double& v){
            static std::random_device rd;
            static std::default_random_engine generator(rd());
            std::normal_distribution<double> distribution(0,sigmoid(v));
            return std::max(0.0,v+distribution(generator));
        }
        static double leakyRelu(const double& x){
            //return (x > 0 ? x:(0.01*x));
            return (x > 0.0 ? 1.0:(0.01));
        }
        static double exponential(const double& v){
            return std::exp(v);
        }
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
