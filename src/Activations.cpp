#include <EvoAI/Activations.hpp>
#include <EvoAI/NeuralNetwork.hpp>
/// TODO
/// 
namespace EvoAI{
    namespace NeuralNetwork{
        double Activations::identity(const double& v){
            return v;
        }
        double Activations::sigmoid(const double& v){
            return (1.0/(1.0+std::exp(-v)));
        }
        double Activations::tanh(const double& v){
            return std::tanh(v);
        }
        double Activations::sinusoid(const double& v){
            return std::sin(v);
        }
        double Activations::relu(const double& v){
            return std::max(0.0,v);
        }
        double Activations::noisyRelu(const double& v){
            static std::random_device rd;
            static std::default_random_engine generator(rd());
            std::normal_distribution<double> distribution(0,sigmoid(v));
            return std::max(0.0,v+distribution(generator));
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
            static std::random_device rd;
            static std::default_random_engine eng(rd());
            static std::normal_distribution<double> d(0.0,v);
            return d(eng);
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
            return 1-std::pow(std::tanh(v),2);
        }
        double Derivatives::sinusoid(const double& v){
            return std::cos(v);
        }
        double Derivatives::relu(const double& v){
            return (v > 0.0 ? 1.0:0.0);
        }
        double Derivatives::softmax(const double& v, NeuralNetwork& nn){
            int index = nn.size()-1; // TODO FIX
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
}