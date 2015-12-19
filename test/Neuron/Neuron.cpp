#include <iostream>
#include <EvoAI/Neuron.hpp>

int main(){
    std::cout << "Testing Neuron..." << std::endl;
    EvoAI::Neuron n(EvoAI::Neuron::INPUT);
    n.setValue(1.0);
    auto val = n.getValue();
    std::cout << "Value from Neuron: " << val << std::endl;
    return 0;
}
