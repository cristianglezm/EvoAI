#include <iostream>
#include <EvoAI.hpp>
#include <utility>
#include <memory>
#include <vector>

int main(){
    using namespace EvoAI;
    double x[4] = {0.0,0.0,1.0,1.0};
    double y[4] = {0.0,1.0,0.0,1.0};
    double truth[4] = {0.0,1.0,1.0,0.0};
    double points = 0.0;
    double minPoints = 500.0;
    for(auto g=0u;g<10250;++g){
        std::unique_ptr<NeuralNetwork> nn = createFeedForwardNN(2,1,2,1,1.0);
        std::cout << "-----------------------------------------" << std::endl;
        for(auto i=0u;i<4;++i){
            std::vector<double> inputs;
            inputs.emplace_back(x[i]);
            inputs.emplace_back(y[i]);
            nn->setInputs(std::move(inputs));
            auto res = nn->run();
            auto answer = std::pow((res[0] - truth[i]),2);
            points += answer;
            std::cout << "x: " << x[i] << ", y: " << y[i] << " : answer: " << res[0] << " Correct Answer: " << truth[i] <<  " Diff: " << answer << std::endl;
            nn->reset();
        }
        points = points / 4;
        if(points < minPoints){
            std::cout << "saving... : " << points << " ||| " << minPoints << std::endl;
            minPoints = points;
            nn->writeToFile("XORNN.json");
        }
        std::cout << "MSE: " << points << std::endl;
        points = 0.0;
    }
    std::cout << "Test Best NN" << std::endl;
    std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>("XORNN.json");
    for(auto i=0u;i<4;++i){
        std::vector<double> inputs;
        inputs.emplace_back(x[i]);
        inputs.emplace_back(y[i]);
        nn->setInputs(std::move(inputs));
        auto res = nn->run();
        auto answer = std::pow((res[0] - truth[i]),2);
        points += answer;
        std::cout << "x: " << x[i] << ", y: " << y[i] << " : answer: " << res[0] << " :bin: " << (res[0] >= 0.5 ? 1:0) << " Correct Answer: " << truth[i] << std::endl;
        nn->reset();
    }
    points = points / 4;
    std::cout << "MSE: " << points << std::endl;
    return 0;
}
