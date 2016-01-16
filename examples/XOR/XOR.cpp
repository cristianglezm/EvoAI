#include <iostream>
#include <EvoAI.hpp>
#include <utility>
#include <memory>
#include <vector>

int main(){
    using namespace EvoAI;
    std::vector<double> x = {0.0,0.0,1.0,1.0};
    std::vector<double> y = {0.0,1.0,0.0,1.0};
    std::vector<double> truth = {0.0,1.0,1.0,0.0};
    double points = 0.0;
    double minPoints = 500.0;
    std::unique_ptr<NeuralNetwork> nn = createFeedForwardNN(2,1,2,1,1.0);
    std::vector<std::vector<double>> inputs;
    std::vector<std::vector<double>> outputs;
    for(auto i=0u;i<4;++i){
        std::vector<double> in;
        std::vector<double> out;
        in.emplace_back(x[i]);
        in.emplace_back(y[i]);
        out.emplace_back(truth[i]);
        inputs.emplace_back(in);
        outputs.emplace_back(out);
    }
    nn->train(std::move(inputs),std::move(outputs),0.7,0.3,5);
    for(auto i=0u;i<4;++i){
        std::vector<double> input;
        input.emplace_back(x[i]);
        input.emplace_back(y[i]);
        nn->setInputs(std::move(input));
        auto res = nn->run();
        auto answer = std::pow((res[0] - truth[i]),2);
        points += answer;
        nn->reset();
        std::cout << "x: " << x[i] << ", y: " << y[i] << " : answer: " << res[0] << " Correct Answer: " << truth[i] <<  " Diff: " << answer << std::endl;
    }
    return 0;
}
