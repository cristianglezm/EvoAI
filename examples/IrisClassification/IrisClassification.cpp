#include <iostream>
#include <EvoAI.hpp>
#include <utility>
#include <memory>
#include <vector>
#include <fstream>

std::vector<std::vector<std::string>> readFile(std::istream& str){
    std::vector<std::vector<std::string>> csvData;
    std::string line;
    std::string cell;
    while(std::getline(str,line)){
        std::vector<std::string> row;
        std::stringstream lineStream(line);
        while(std::getline(lineStream,cell,',')){
            row.emplace_back(cell);
        }
        csvData.emplace_back(row);
    }
    return std::move(csvData);
}

std::pair<EvoAI::NeuralNetwork::trainingFormat,EvoAI::NeuralNetwork::trainingFormat> createTrainingSets(std::vector<std::vector<std::string>> dataset, const std::size_t& start, const std::size_t& end){
    EvoAI::NeuralNetwork::trainingFormat irisTrainingDataInput;
    EvoAI::NeuralNetwork::trainingFormat irisTrainingDataOutput;
    for(auto i=start;i<end;++i){
        std::vector<double> in;
        std::vector<double> out;
        auto size = dataset[i].size()-1;
        for(auto j=0u;j<size;++j){
            in.emplace_back(std::stod(dataset[i][j]));
        }
        if(dataset[i][size] == "Iris-setosa"){
            out.emplace_back(1.0);
            out.emplace_back(0.0);
            out.emplace_back(0.0);
        }else if(dataset[i][size] == "Iris-versicolor"){
            out.emplace_back(0.0);
            out.emplace_back(1.0);
            out.emplace_back(0.0);
        }else if(dataset[i][size] == "Iris-virginica"){
            out.emplace_back(0.0);
            out.emplace_back(0.0);
            out.emplace_back(1.0);
        }
        irisTrainingDataInput.emplace_back(in);
        irisTrainingDataOutput.emplace_back(out);
    }
    return std::move(std::pair<EvoAI::NeuralNetwork::trainingFormat,EvoAI::NeuralNetwork::trainingFormat>(irisTrainingDataInput,irisTrainingDataOutput));
}
int main(int argc,char **argv){
    std::fstream csv("iris.data.csv");
    std::cout << "Processing CSV file..." << std::endl;
    auto irisData = readFile(csv);
    std::mt19937 g(std::random_device{}());
    std::cout << "Randomizing Data Order..." << std::endl;
    std::shuffle(std::begin(irisData),std::end(irisData),g);
    std::cout << "Creating Test dataSets..." << std::endl;
    auto testSets = createTrainingSets(irisData,irisData.size()/2,irisData.size());
    std::unique_ptr<EvoAI::NeuralNetwork> nn = nullptr;
    if(argc > 1){
        nn = std::make_unique<EvoAI::NeuralNetwork>(std::string(argv[1]));
    }else{
        std::cout << "Training Neural Network..." << std::endl;
        nn = EvoAI::createFeedForwardNN(4,2,3,3,1.0);
        do{
            auto trainingSets = createTrainingSets(irisData,0,irisData.size()/1.2);
            nn->train(std::move(trainingSets.first),std::move(trainingSets.second),1.7,0.4,15);
            std::cout << "MSE: " << nn->getMSE() << std::endl;
        }while(nn->getMSE() > 0.001);
        nn->writeToFile("IrisClassification.json");
    }
    std::cout << "Processing Test Classification..." << std::endl;
    auto accuracy = 0.0;
    for(auto i=0u;i<testSets.first.size();++i){
        nn->setInputs(std::move(testSets.first[i]));
        auto outputs = nn->run();
        std::string nnOut = "", expectedOut = "";
        if(outputs[0] >= 0.5){
            nnOut = "Iris-setosa";
        }else if(outputs[1] >= 0.5){
            nnOut = "Iris-versicolor";
        }else if(outputs[2] >= 0.5){
            nnOut = "Iris-virginica";
        }
        if(testSets.second[i][0] >= 0.5){
            expectedOut = "Iris-setosa";
        }else if(testSets.second[i][1] >= 0.5){
            expectedOut = "Iris-versicolor";
        }else if(testSets.second[i][2] >= 0.5){
            expectedOut = "Iris-virginica";
        }
        if(nnOut == expectedOut){
            accuracy += 1;
        }
        std::cout << nnOut << " : " << expectedOut << std::endl;
    }
    accuracy /= testSets.second.size();
    accuracy *= 100;
    std::cout << "Accuracy: " << accuracy << "%" << std::endl;
    return 0;
}
