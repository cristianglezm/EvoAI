#include <EvoAI/Loss/MultiClassCrossEntropy.hpp>
#include <algorithm>

namespace EvoAI{
    namespace Loss{
        double MultiClassCrossEntropy::operator()(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept{
            double sum = 0.0;
            const double epsilon = 1e-15;
            for(auto i=0u;i<expectedOutputs.size();++i){
                auto out = std::clamp(outputs[i], epsilon, 1.0 - epsilon);
                sum += expectedOutputs[i] * std::log(out);
            }
            return -(sum/outputs.size());
        }
        std::vector<double> MultiClassCrossEntropy::backward(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept{
            std::vector<double> grads;
            grads.reserve(outputs.size());
            const double epsilon = 1e-15;
            for(auto i=0u;i<outputs.size();++i){
                auto out = std::clamp(outputs[i], epsilon, 1.0 - epsilon);
                grads.emplace_back((1.0 - expectedOutputs[i]) / (1.0 - out) - expectedOutputs[i] / out);
            }
            return grads;
        }
    }
}