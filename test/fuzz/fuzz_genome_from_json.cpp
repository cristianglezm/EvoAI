// libFuzzer harness for EvoAI::Genome's JSON constructor and
// Genome::makePhenotype().
//
// makePhenotype() is noexcept and used inside HyperNeat's evolutionary
// loops, so it can't throw its way out of a malformed genome the way the
// NeuralNetwork/Graph constructors now do - it has to degrade gracefully
// instead (see the empty-genome fix this harness exists to guard). This
// exercises both the deserialization boundary and the conversion to a
// NeuralNetwork phenotype in one pass, since a genome that deserializes
// fine can still be structurally degenerate (e.g. no node chromosomes at
// all) in a way that only bites during the conversion.
#include <EvoAI/Genome.hpp>
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
        return 0;
    }
    if(!v.isObject()){
        return 0;
    }
    try{
        EvoAI::Genome g(v.getObject());
        auto nn = EvoAI::Genome::makePhenotype(g);
        if(nn.size() > 0){
            std::vector<double> ins(nn[0].size(), 0.5);
            nn.setInputs(ins);
            nn.run();
        }
    }catch(const std::exception&){
        return 0;
    }
    return 0;
}
