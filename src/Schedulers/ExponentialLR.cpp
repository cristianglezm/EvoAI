#include <EvoAI/Schedulers/ExponentialLR.hpp>

namespace EvoAI{
        ExponentialLR::ExponentialLR(double gamma)
        : m_gamma(gamma){}
        ExponentialLR::ExponentialLR(JsonBox::Object o)
        : m_gamma(o["gamma"].tryGetDouble(1.0)){}
        JsonBox::Value ExponentialLR::toJson() const noexcept{
            JsonBox::Object o;
            o["name"] = "ExponentialLR";
            o["gamma"] = m_gamma;
            return o;
        }
        double ExponentialLR::operator()(double lr, [[maybe_unused]] std::size_t epoch) const noexcept{
            return lr * m_gamma;
        }
}