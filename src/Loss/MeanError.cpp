#include <EvoAI/Loss/MeanError.hpp>
#include <algorithm>

namespace EvoAI{
    namespace Loss{
        double MeanError::operator()(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept{
            double totalError = 0.0;
            for(auto i=0u;i<outputs.size();++i){
                totalError += outputs[i] - expectedOutputs[i];
            }
            return totalError / outputs.size();
        }
        std::vector<double> MeanError::backward([[maybe_unused]] const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept{
            std::vector<double> grads;
            grads.reserve(outputs.size());
            for (auto i=0u;i<outputs.size();++i){
                grads.emplace_back(1.0 / outputs.size());
            }
            return grads;
        }
    }
}