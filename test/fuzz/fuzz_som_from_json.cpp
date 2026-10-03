// libFuzzer harness for EvoAI::SelfOrganizingMap's JSON constructor, followed by
// classify(), getPrototype(), a one-epoch train() and toJson(): a map that loads but
// can't be used safely is the case worth finding.
#include <EvoAI/SelfOrganizingMap.hpp>
#include <JsonBox.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

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
    try{
        EvoAI::SelfOrganizingMap<> som(v.getObject());
        // a loaded map is only as big as the weights the input actually carried
        std::vector<double> sample(som.getInputDim(), 0.5);
        auto match = som.classify(sample);
        if(match){
            (void)som.getPrototype(match->first, match->second);
        }
        som.train({sample}, 1u, 0.5, 1.0,
                  EvoAI::Scheduler<EvoAI::ConstantLR>(EvoAI::ConstantLR{}),
                  EvoAI::Scheduler<EvoAI::ConstantLR>(EvoAI::ConstantLR{}));
        (void)som.toJson();
    }catch(const std::exception&){
    }
    return 0;
}
