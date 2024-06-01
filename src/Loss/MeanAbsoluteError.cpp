#include <EvoAI/Loss/MeanAbsoluteError.hpp>
#include <algorithm>

namespace EvoAI{
    namespace Loss{
        double MeanAbsoluteError::operator()(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept{
            double totalError = 0.0;
            for(auto i=0u;i<outputs.size();++i){
                totalError += std::abs(outputs[i] - expectedOutputs[i]);
            }
            return totalError / outputs.size();
        }
        std::vector<double> MeanAbsoluteError::backward(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept{
            std::vector<double> grads;
            grads.reserve(outputs.size());
            for(auto i=0u;i<outputs.size();++i){
                if(outputs[i] > expectedOutputs[i]){
                    grads.emplace_back(1.0 / outputs.size());
                }else{
                    grads.emplace_back(-1.0 / outputs.size());
                }
            }
            return grads;
        }
    }
}