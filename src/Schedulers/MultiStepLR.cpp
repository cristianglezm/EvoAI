#include <EvoAI/Schedulers/MultiStepLR.hpp>

namespace EvoAI{
        MultiStepLR::MultiStepLR(std::vector<std::size_t>&& steps, double gamma)
        : m_steps(std::forward<std::vector<std::size_t>>(steps))
        , m_gamma(gamma)
        , m_counter{0u}{}
        MultiStepLR::MultiStepLR(JsonBox::Object o)
        : m_steps{}
        , m_gamma(o["gamma"].tryGetDouble(1.0))
        , m_counter{std::stoul(o["counter"].tryGetString("0"))}{
            auto& arr = o["steps"].getArray();
            m_steps.reserve(arr.size());
            for(auto& s:arr){
                m_steps.emplace_back(std::stoul(s.getString()));
            }
        }
        JsonBox::Value MultiStepLR::toJson() const noexcept{
            JsonBox::Object o;
            o["name"] = "MultiStepLR";
            JsonBox::Array arr;
            arr.reserve(m_steps.size());
            for(auto& s:m_steps){
                arr.emplace_back(std::to_string(s));
            }
            o["steps"] = arr;
            o["gamma"] = m_gamma;
            o["counter"] = std::to_string(m_counter);
            return o;
        }
        double MultiStepLR::operator()(double lr, std::size_t epoch) const noexcept{
            if(epoch == m_steps[m_counter]){
                if(m_counter < m_steps.size()){
                    ++m_counter;
                }
                return lr * m_gamma;
            }
            return lr;
        }
}