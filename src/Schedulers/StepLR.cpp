#include <EvoAI/Schedulers/StepLR.hpp>

namespace EvoAI{
        StepLR::StepLR(std::size_t step, double gamma)
        : m_step(step)
        , m_gamma(gamma){}
        StepLR::StepLR(JsonBox::Object o)
        : m_step(std::stoull(o["step"].tryGetString("10")))
        , m_gamma(o["gamma"].tryGetDouble(1.0)){}
        JsonBox::Value StepLR::toJson() const noexcept{
            JsonBox::Object o;
            o["name"] = "StepLR";
            o["step"] = std::to_string(m_step);
            o["gamma"] = m_gamma;
            return o;
        }
        double StepLR::operator()(double lr, std::size_t epoch) const noexcept{
            if(epoch % m_step == 0){
                return lr * m_gamma;
            }
            return lr;
        }
}