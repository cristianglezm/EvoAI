// libFuzzer harness for EvoAI's LR-Scheduler algorithms' JSON
// constructors (ConstantLR, ExponentialLR, MultiplicativeLR, MultiStepLR,
// StepLR).
#include <EvoAI/Schedulers.hpp>
#include <JsonBox.h>
#include <cstddef>
#include <cstdint>
#include <string>

namespace{
    template<class Algo>
    void exercise(Algo&& algo){
        for(std::size_t epoch = 0; epoch < 4; ++epoch){
            algo(1.0, epoch);
        }
    }
}

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size){
    std::string input(reinterpret_cast<const char*>(data), size);
    JsonBox::Value v;
    try{
        v.loadFromString(input);
    }catch(const std::exception&){
        return 0;
    }
    if(!v.isObject()){
        return 0;
    }
    auto o = v.getObject();
    try{
        exercise(EvoAI::ConstantLR(o));
    }catch(const std::exception&){
    }
    try{
        exercise(EvoAI::ExponentialLR(o));
    }catch(const std::exception&){
    }
    try{
        exercise(EvoAI::MultiplicativeLR(o));
    }catch(const std::exception&){
    }
    try{
        exercise(EvoAI::MultiStepLR(o));
    }catch(const std::exception&){
    }
    try{
        exercise(EvoAI::StepLR(o));
    }catch(const std::exception&){
    }
    return 0;
}
