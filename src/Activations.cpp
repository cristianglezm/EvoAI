#include <EvoAI/Activations.hpp>
#include <EvoAI/NeuralNetwork.hpp>

#include <chrono>
/// @todo review activations and derivatives.
namespace EvoAI{
    double Activations::identity(const double& v){
        return v;
    }
    double Activations::sigmoid(const double& v){
        return (1.0/(1.0+std::exp(-v)));
    }
    double Activations::tanh(const double& v){
        return ((std::tanh(v) + 1)/2);
    }
    double Activations::sinusoid(const double& v){
        return std::sin(v);
    }
    double Activations::cosine(const double& v){
        return std::cos(v);
    }
    double Activations::tan(const double& v){
        return std::tan(v);
    }
    double Activations::relu(const double& v){
        return std::max(0.0,v);
    }
    double Activations::noisyRelu(const double& v){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        static std::mt19937 rEngine(seed);
        std::normal_distribution<double> distribution(0,sigmoid(v));
        return std::max(0.0,v+distribution(rEngine));
    }
    double Activations::leakyRelu(const double& x){
        //return (x > 0 ? x:(0.01*x));
        return (x > 0.0 ? 1.0:(0.01));
    }
    double Activations::exponential(const double& v){
        return std::exp(v);
    }
    double Activations::softmax(const double& v, NeuralNetwork& nn){
        int index = nn.size()-1;
        auto& outputs = nn[index];
        auto sum = 0.0;
        for(auto& n:outputs.getNeurons()){
            sum += std::exp(n.getOutput());
        }
        return (std::exp(v) / (sum));
    }
    double Activations::gaussian(const double& v){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        static std::mt19937 rEngine(seed);
        static std::normal_distribution<double> d(0.0,v);
        return d(rEngine);
    }
    double Activations::modulus(const double& v){
        return std::fmod(v,1.0);
    }
// derivatives
    double Derivatives::identity(const double& v){
        return 1;
    }
    double Derivatives::sigmoid(const double& v){
        auto y = Activations::sigmoid(v);
        return (y*(1-y));
    }
    double Derivatives::tanh(const double& v){
        return ((1-std::pow(std::tanh(v),2))/2);
    }
    double Derivatives::sinusoid(const double& v){
        return std::cos(v);
    }
    double Derivatives::relu(const double& v){
        return (v > 0.0 ? 1.0:0.0);
    }
    double Derivatives::softmax(const double& v, NeuralNetwork& nn){
        int index = nn.size()-1; /// @todo FIX softmax derivative
        auto& outputs = nn[index];
        auto sum = 0.0;
        for(auto& n:outputs.getNeurons()){
            sum += v*((-v*n.getOutput()) - n.getOutput());
        }
        return sum;
    }
    double Derivatives::gaussian(const double& v){
        return (std::sqrt(2/3.14159265359)*(-std::exp(std::pow(-v,2))));
    }
}
