#include <EvoAI/Schedulers/MultiplicativeLR.hpp>

namespace EvoAI{
        MultiplicativeLR::MultiplicativeLR(double factor)
        : m_factor(factor){}
        MultiplicativeLR::MultiplicativeLR(JsonBox::Object o)
        : m_factor{o["factor"].tryGetDouble(1.0)}{}
        JsonBox::Value MultiplicativeLR::toJson() const noexcept{
            JsonBox::Object o;
            o["name"] = "MultiplicativeLR";
            o["factor"] = m_factor;
            return o;
        }
        double MultiplicativeLR::operator()(double lr, [[maybe_unused]] std::size_t epoch) const noexcept{
            return lr * m_factor;
        }
}