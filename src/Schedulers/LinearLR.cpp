#include <algorithm>
#include <EvoAI/Schedulers/LinearLR.hpp>
#include <EvoAI/Utils/TypeUtils.hpp>

namespace EvoAI{
        LinearLR::LinearLR(std::size_t step, double decrement, double minValue)
        : m_step(step)
        , m_decrement(decrement)
        , m_minValue(minValue){}
        LinearLR::LinearLR(JsonBox::Object o)
        : m_step(safeParseUInt<std::size_t>(o["step"].getString(), 10))
        , m_decrement(o["decrement"].tryGetDouble(0.01))
        , m_minValue(o["minValue"].tryGetDouble(0.0)){}
        JsonBox::Value LinearLR::toJson() const noexcept{
            JsonBox::Object o;
            o["name"] = "LinearLR";
            o["step"] = std::to_string(m_step);
            o["decrement"] = m_decrement;
            o["minValue"] = m_minValue;
            return o;
        }
        double LinearLR::operator()(double lr, std::size_t epoch) const noexcept{
            if(epoch % m_step == 0){
                return std::max(m_minValue, lr - m_decrement);
            }
            return lr;
        }
}
