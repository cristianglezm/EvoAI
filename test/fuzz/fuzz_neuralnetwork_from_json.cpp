// libFuzzer harness for EvoAI::NeuralNetwork's JSON constructor.
//
// EvoAI::NeuralNetwork(JsonBox::Object) is the untrusted-input boundary:
// anything that saves/loads a NeuralNetwork goes through it.
// This harness parses the fuzzer's bytes as JSON,
// constructs a NeuralNetwork from whatever object results, and exercises
// it the way a real consumer would (setInputs + run + backward) so bugs
// that only manifest once the network is *used*, not just constructed,
// still get found.
#include <EvoAI/NeuralNetwork.hpp>
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
        // JsonBox itself rejecting malformed/non-UTF8 input is expected
        // and not a finding - it's a different, well-defined failure mode
        // than a crash inside EvoAI's own parsing.
        return 0;
    }
    if(!v.isObject()){
        return 0;
    }
    try{
        EvoAI::NeuralNetwork nn(v.getObject());
        std::vector<double> ins(nn.size() > 0 ? nn[0].size() : 0, 0.5);
        nn.setInputs(ins);
        auto outputs = nn.run();
        std::vector<double> gradientLoss(outputs.size(), 0.1);
        nn.backward(std::move(gradientLoss));
    }catch(const std::exception&){
        // A well-defined, caught exception on malformed input is the
        // whole point of the hardening pass this harness exists to
        // guard - not a finding.
        return 0;
    }
    return 0;
}
