#include <EvoAI/Schedulers/ConstantLR.hpp>

namespace EvoAI{
        ConstantLR::ConstantLR([[maybe_unused]] JsonBox::Object o){}
        JsonBox::Value ConstantLR::toJson() const noexcept{
            JsonBox::Object o;
            o["name"] = "ConstantLR";
            return o;
        }
        double ConstantLR::operator()(double lr, [[maybe_unused]] std::size_t epoch) const noexcept{
            return lr;
        }
}