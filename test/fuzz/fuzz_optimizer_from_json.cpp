// libFuzzer harness for EvoAI's Optimizer algorithms' JSON constructors
// (SGD, Adam, Muon).
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Optimizers/SGD.hpp>
#include <EvoAI/Optimizers/Adam.hpp>
#include <EvoAI/Optimizers/Muon.hpp>
#include <JsonBox.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace{
    // Fixed, non-fuzzed 2-2-2 fully connected network, just to get a
    // real std::vector<Connection*> the way a caller would.
    EvoAI::NeuralNetwork makeFixedNetwork(){
        EvoAI::NeuralNetwork nn(2, 1, {2}, 2, 0.0);
        nn.addConnection(EvoAI::Connection(EvoAI::Link(0,0), EvoAI::Link(1,0), 0.5));
        nn.addConnection(EvoAI::Connection(EvoAI::Link(0,1), EvoAI::Link(1,1), 0.5));
        nn.addConnection(EvoAI::Connection(EvoAI::Link(1,0), EvoAI::Link(2,0), 0.5));
        nn.addConnection(EvoAI::Connection(EvoAI::Link(1,1), EvoAI::Link(2,1), 0.5));
        return nn;
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
    auto nn = makeFixedNetwork();
    try{
        EvoAI::SGD sgd(o, nn.getParameters());
        sgd(0.01, 1);
    }catch(const std::exception&){
        // A well-defined, caught exception on malformed input is fine -
        // not a finding.
    }
    try{
        EvoAI::Adam adam(o, nn.getParameters());
        adam(0.01, 1);
    }catch(const std::exception&){
    }
    try{
        EvoAI::Muon muon(o, nn.getParameters());
        muon(0.01, 1);
    }catch(const std::exception&){
    }
    return 0;
}
