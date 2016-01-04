#include <iostream>
#include <EvoAI.hpp>
#include <utility>
#include <memory>
#include <vector>

int main(){
    using namespace EvoAI;
    std::unique_ptr<NeuralNetwork> nn = createXORNeuralNetwork();
    double x[4] = {0.0,0.0,1.0,1.0};
    double y[4] = {0.0,1.0,0.0,1.0};
    double truth[4] = {0.0,1.0,1.0,0.0};
    for(auto i=0u;i<4;++i){
        std::vector<double> inputs;
        inputs.emplace_back(x[i]);
        inputs.emplace_back(y[i]);
        nn->setInputs(std::move(inputs));
        auto res = nn->run();
        std::cout << "x: " << x[i] << ", y: " << y[i] << " : answer: " << res[0] << " Correct Answer: " << truth[i] << std::endl;
        nn->reset();
    }
    return 0;
}
