#include <EvoAI/Activations.hpp>
#include <EvoAI/NeuralNetwork.hpp>

#include <chrono>
/// @todo review activations and derivatives.
namespace EvoAI{
    double Activations::identity(const double& v){
        return v;
    }
    double Activations::sigmoid(const double& v){
        return (1.0/(1.0+std::exp(-1.0 * v)));
    }
    double Activations::steepenedSigmoid(const double& v){
        return (1.0/(1.0+std::exp(-4.9 * v)));
    }
    double Activations::tanh(const double& v){
        return std::tanh(v);
    }
    double Activations::sinusoid(const double& v){
        return std::sin(2.0*v);
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
        Neuron* n = nullptr;
        auto maxEl = std::max_element(std::begin(outputs.getNeurons()),std::end(outputs.getNeurons()),
            [&](Neuron& n1, Neuron& n2){
               if(v == n1.getSum()){
                    n = &n1;
               }
               return (n1.getSum() < n2.getSum());
            });
        auto sum = 0.0;
        std::transform(std::begin(outputs.getNeurons()),std::end(outputs.getNeurons()), std::begin(outputs.getNeurons()),
            [&](Neuron& ne){
                auto ex = std::exp(ne.getSum() - maxEl->getSum());
                sum += ex;
                ne.setSum(ex);
                return ne;
            });
        std::transform(std::begin(outputs.getNeurons()),std::end(outputs.getNeurons()),std::begin(outputs.getNeurons()),
            [&](Neuron& ne){
                if(std::isnan(sum) || sum < std::numeric_limits<double>::min()){
                    ne.setOutput(1.0/sum);
                }else{
                    ne.setOutput(ne.getSum() / sum);
                }
                return ne;
            });
        if(n!=nullptr){
            return n->getOutput();
        }else{
            return v;
        }
    }
    double Activations::gaussian(const double& v){
        return std::exp(-std::pow(2.5*v,2));
    }
    double Activations::modulus(const double& v){
        return std::fmod(v,1.0);
    }
    double Activations::square(const double& v){
        return (v*v);
    }
    double Activations::cube(const double& v){
        return (v*v*v);
    }
// derivatives
    double Derivatives::identity(const double& v){
        return 1;
    }
    double Derivatives::sigmoid(const double& v){
        //auto y = Activations::sigmoid(v);
        return (v*(1-v));
    }
    double Derivatives::steepenedSigmoid(const double& v){
        double y = std::exp(-4.9*v);
        return std::pow(y*4.9/(1+y),2);
    }
    double Derivatives::tanh(const double& v){
        return (1.0 - v * v);
    }
    double Derivatives::sinusoid(const double& v){
        return std::cos(2.0*v);
    }
    double Derivatives::cosine(const double& v){
        return std::sinh(v);
    }
    double Derivatives::tan(const double& v){
        auto x = 1/std::cos(v);
        return (x * x);
    }
    double Derivatives::relu(const double& v){
        return (v > 0.0 ? 1.0:0.0);
    }
    double Derivatives::softmax(const double& v, NeuralNetwork& nn){
        return 1;
    }
    double Derivatives::gaussian(const double& v){
        return std::exp(std::pow(2.5*v,2.0) * 12.5 * v);
    }
    double Derivatives::square(const double& v){
        return (2*v);
    }
    double Derivatives::cube(const double& v){
        return (v/3);
    }
}
