#include <EvoAI/Loss/MeanSquaredError.hpp>
#include <algorithm>

namespace EvoAI{
    namespace Loss{
        double MeanSquaredError::operator()(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept{
            double totalError = 0.0;
            for(auto i=0u;i<outputs.size();++i){
                auto error = outputs[i] - expectedOutputs[i];
                totalError += error * error;
            }
            return totalError / outputs.size();
        }
        std::vector<double> MeanSquaredError::backward(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept{
            std::vector<double> grads;
            grads.reserve(outputs.size());
            for(auto i=0u;i<outputs.size();++i){
                grads.emplace_back((2.0 * (outputs[i] - expectedOutputs[i])) / outputs.size());
            }
            return grads;
        }
    }
}