#include <EvoAI/Activations.hpp>
#include <EvoAI/NeuralNetwork.hpp>

namespace EvoAI{
    double Activations::identity(double v) noexcept{
        return v;
    }
    double Activations::sigmoid(double v) noexcept{
        return (1.0 / (1.0 + std::exp(-v)));
    }
    double Activations::steepenedSigmoid(double v) noexcept{
        return (1.0 / (1.0+std::exp(-(4.9 * v))));
    }
    double Activations::swish(double x, double b) noexcept{
        return x / (1.0 + std::exp(-(b * x)));
    }
    double Activations::tanh(double v) noexcept{
        return std::tanh(v);
    }
    double Activations::sinusoid(double v) noexcept{
        return std::sin(v);
    }
    double Activations::cosine(double v) noexcept{
        return std::cos(v);
    }
    double Activations::tan(double v) noexcept{
        return std::tan(v);
    }
    double Activations::relu(double v) noexcept{
        return std::max(0.0,v);
    }
    double Activations::noisyRelu(double v) noexcept{
        auto eng = randomGen().getEngine();
        std::normal_distribution<double> dis(0.0, 1.0);
        auto noise = dis(eng) * 0.01;
        return std::max(0.0,v + noise);
    }
    double Activations::leakyRelu(double x) noexcept{
        return (x > 0.0 ? x:(0.01*x));
    }
    double Activations::exponential(double v) noexcept{
        return std::exp(v);
    }
    void Activations::softmax(NeuronLayer& outputs) noexcept{
        auto max = std::max_element(std::begin(outputs.getNeurons()), std::end(outputs.getNeurons()), 
            [](auto& a, auto& b){
                return a.getOutput() < b.getOutput();
        });
        for(auto& n:outputs.getNeurons()){
            n.setOutput(std::exp(n.getOutput() - max->getOutput()));
        }
        auto totalSum = std::accumulate(std::begin(outputs.getNeurons()), std::end(outputs.getNeurons()), 0.0, 
                        [](auto& a, auto& b){
                            return a + b.getOutput();
        });
        for(auto& n:outputs.getNeurons()){
            n.setOutput(n.getOutput() / totalSum);
        }
    }
    double Activations::gaussian(double v) noexcept{
        const double sqpi = std::sqrt(2.0 / EvoAI::PI);
        double cdf = 0.5 * (1.0 + std::tanh(sqpi * (v + 0.044715 * std::pow(v, 3))));
        return v * cdf;
    }
    double Activations::modulus(double v, double div) noexcept{
        return std::fmod(v, div);
    }
    double Activations::square(double v) noexcept{
        return (v*v);
    }
    double Activations::cube(double v) noexcept{
        return (v*v*v);
    }
    double Activations::softplus(double v) noexcept{
        double abs_v = std::abs(v);
        return std::log(1.0 + std::exp(-abs_v)) + std::max(0.0, v);
    }
    double Activations::clamp(double v) noexcept{
        return std::clamp(v, -1.0, 1.0);
    }
    double Activations::inv(double v) noexcept{
        return -v;
    }
    double Activations::log(double v) noexcept{
        return std::log(v);
    }
    double Activations::abs(double v) noexcept{
        return std::abs(v);
    }
    double Activations::hat(double v) noexcept{
        return std::max(0.0, 1.0 - std::abs(v));
    }
// derivatives
    double Derivatives::identity([[maybe_unused]] double v) noexcept{
        return 1.0;
    }
    double Derivatives::modulus(double v, double div) noexcept{
        if(v > 0.0 && v < div){
            return 1.0;
        }else if(v < 0.0 && v > -div){
            return -1.0;
        }
        return 0.0;
    }
    double Derivatives::sigmoid(double v) noexcept{
        v = Activations::sigmoid(v);
        return (v * (1.0 - v));
    }
    double Derivatives::steepenedSigmoid(double v) noexcept{
        v = Activations::sigmoid(v);
        return v * (1.0 - v);
    }
    double Derivatives::swish(double x, [[maybe_unused]] double b) noexcept{
        x = Activations::sigmoid(x);
        return (x + Derivatives::sigmoid(x) * (1.0 - x));
    }
    double Derivatives::tanh(double v) noexcept{
        double x = std::cosh(v);
        return (1.0 / (x * x));
    }
    double Derivatives::sinusoid(double v) noexcept{
        return std::cos(v);
    }
    double Derivatives::cosine(double v) noexcept{
        return -std::sin(v);
    }
    double Derivatives::tan(double v) noexcept{
        double x = std::cos(v);
        return (1.0 / (x * x));
    }
    double Derivatives::relu(double v) noexcept{
        return (v > 0.0 ? 1.0:0.0);
    }
    double Derivatives::noisyRelu(double v) noexcept{
        auto eng = randomGen().getEngine();
        std::normal_distribution<double> dis(0.0, 1.0);
        auto noise = dis(eng) * 0.01;
        return ((v + noise * 0.01) > 0.0 ? 1.0:0.0);
    }
    double Derivatives::leakyRelu(double x) noexcept{
        return (x > 0.0 ? 1.0:0.0);
    }
    double Derivatives::exponential(double v) noexcept{
        return std::exp(v);
    }
    void Derivatives::softmax(NeuronLayer& outputs) noexcept{
        for(auto& nrn:outputs.getNeurons()){
            nrn.setGradient(nrn.getOutput() * (1.0 - nrn.getOutput()));
        }
    }
    double Derivatives::gaussian(double v) noexcept{
        const double sqpi = std::sqrt(2.0 / EvoAI::PI);
        double exp = std::exp(-0.5 * std::pow(v, 2));
        return 0.5 * (1 + std::erf(sqpi * (v + 0.044715 * std::pow(v, 3)))) + v * exp / std::sqrt(2.0 * EvoAI::PI);
    }
    double Derivatives::square(double v) noexcept{
        return 2.0 * v;
    }
    double Derivatives::cube(double v) noexcept{
        return 3.0 * v * v;
    }
    double Derivatives::softplus(double v) noexcept{
        return 1.0 / (1.0 + std::exp(-v));
    }
    double Derivatives::clamp(double v) noexcept{
        if(v < -1.0 || v > 1.0){
            return 0.0;
        }
        return 1.0;
    }
    double Derivatives::inv([[maybe_unused]] double v) noexcept{
        return -1.0;
    }
    double Derivatives::log(double v) noexcept{
        return 1.0 / v;
    }
    double Derivatives::abs(double v) noexcept{
        return std::abs(v) / v;
    }
    double Derivatives::hat(double v) noexcept{
        return (v > 0.0 ? (1.0 - (std::abs(v) / v)) : 0.0);
    }
}
