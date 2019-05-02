#include <iostream>
#include <EvoAI.hpp>
#include <utility>
#include <memory>
#include <vector>
#include <fstream>

std::vector<std::vector<std::string>> readCSVFile(std::istream& str);

void usage();
void evolveNeat(float errorThreehold, std::vector<std::vector<std::string>>& irisData);
void evolveHyperneat(float errorThreehold,std::vector<std::vector<std::string>>& irisData);
std::pair<EvoAI::NeuralNetwork::trainingFormat,
            EvoAI::NeuralNetwork::trainingFormat> createTrainingSets(std::vector<std::vector<std::string>> dataset, const std::size_t& start, const std::size_t& end);

void normalizeData(EvoAI::NeuralNetwork::trainingFormat& data);

int main(int argc,char **argv){
    if(argc > 2){
        std::string dataInput(argv[1]);
        std::fstream csv(dataInput);
        std::cout << "Processing CSV file..." << std::endl;
        auto irisData = readCSVFile(csv);
        std::mt19937 g(std::random_device{}());
        std::cout << "Randomizing Data Order..." << std::endl;
        std::shuffle(std::begin(irisData),std::end(irisData),g);
        std::unique_ptr<EvoAI::NeuralNetwork> nn = nullptr;
        std::string opt(argv[2]);
        std::string filename("");
        float errorThreehold = 20.0;
        bool hyperneat = false;
        if(argc > 3){
            filename = std::string(argv[3]);
        }
        if(filename.empty() && (opt!="-e" || opt!="--evolve")){
            std::cout << "creating neural network..." << std::endl;
            nn = EvoAI::createFeedForwardNN(4,2,10,3,0.8);
            (*nn)[1].setActivationType(EvoAI::Neuron::ActivationType::RELU);
            (*nn)[2].setActivationType(EvoAI::Neuron::ActivationType::RELU);
            (*nn)[3].setActivationType(EvoAI::Neuron::ActivationType::SOFTMAX);
        }else{
            if(opt == "-e" || opt == "--evolve"){
                errorThreehold = std::stof(std::string(argv[3]));
                std::string hn = "";
                if(argv[4] != nullptr){
                    hn = std::string(argv[4]);
                    if(hn == "hn"){
                        hyperneat = true;
                    }
                }
            }else{
                std::cout << "loading neural network..." << std::endl;
                nn = std::make_unique<EvoAI::NeuralNetwork>(filename);
            }
        }
        if(opt == "-t" || opt == "--train"){
            std::cout << "Training Neural Network..." << std::endl;
            do{
                auto trainingSets = createTrainingSets(irisData,0,irisData.size()/2);
                normalizeData(trainingSets.first);
                nn->train(std::move(trainingSets.first),std::move(trainingSets.second),0.2,0.02,150);
                std::cout << "\rMSE: " << nn->getMSE() << " ";
                std::flush(std::cout);
                nn->writeToFile("IrisClassification.json");
            }while(nn->getMSE() > 0.02);
            std::cout << std::endl;
        }else if(opt == "-c" || opt == "--classify"){
            std::cout << "Creating Test dataSets..." << std::endl;
            auto testSets = createTrainingSets(irisData,irisData.size()/2,irisData.size());
            std::cout << "Processing Test Classification..." << std::endl;
            auto error = 0.0;
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
                if(nnOut != expectedOut){
                    error += 1;
                }
                std::cout << nnOut << " : "<< expectedOut << std::endl;
                std::cout << "\tIris-setosa" << ":" << " - " << (outputs[0] * 100) << "%" << std::endl;
                std::cout << "\tIris-versicolor" << ":" << " - " << (outputs[1] * 100) << "%" << std::endl;
                std::cout << "\tIris-virginica" << ":" << " - " << (outputs[2] * 100) << "%" << std::endl;
            }
            error /= testSets.second.size();
            error *= 100;
            auto accuracy = 100 - error;
            std::cout << "Error: " << error << "%" << " Accuracy: " << accuracy << "%" << std::endl;
        }else if(opt == "-e" || opt == "--evolve"){
            std::cout << " It will stop when error is less than " << errorThreehold << std::endl;
            if(hyperneat){
                std::cout << "Evolving Population of HyperNEAT Neural Network" << std::endl;
                evolveHyperneat(errorThreehold,irisData);
            }else{
                std::cout << "Evolving Population of NEAT Neural Network" << std::endl;
                evolveNeat(errorThreehold,irisData);
            }
        }else if(opt == "-h" || opt == "--help"){
            usage();
        }
    }else{
        usage();
    }
    return 0;
}

std::vector<std::vector<std::string>> readCSVFile(std::istream& str){
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
    return csvData;
}
void usage(){
    std::cout << "usage: IrisClassification <dataInput> <options> <filename>" << std::endl;
    std::cout << "dataInput is a cvs file with this format <sepal length>,<sepal width>,<petal length>,<petal width>,Iris-setosa || Iris-versicolor || Iris-virginica" << std::endl;
    std::cout << "filename should be a json file of a neural network if empty it will create a random nn." << std::endl;
    std::cout << "filename with option -e or --evolve is an error Threehold to stop evolving the population." << std::endl;
    std::cout << "-t, --train\t\tWill train the network specified." << std::endl;
    std::cout << "-c, --classify\t\tWill test the network with the test data." << std::endl;
    std::cout << "-e, --evolve <errorTreehold> <hn>\t\tWill evolve a population and select the best." << std::endl;
    std::cout << "-h, --help\t\tthis menu." << std::endl;
}
std::pair<EvoAI::NeuralNetwork::trainingFormat,
            EvoAI::NeuralNetwork::trainingFormat> createTrainingSets(std::vector<std::vector<std::string>> dataset, const std::size_t& start, const std::size_t& end){
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
    return std::move(std::make_pair(irisTrainingDataInput,irisTrainingDataOutput));
}
void normalizeData(EvoAI::NeuralNetwork::trainingFormat& data){
    /**
        1. sepal length in cm
        2. sepal width in cm
        3. petal length in cm
        4. petal width in cm
    */
    std::vector<double*> sepalLength;
    std::vector<double*> sepalWidth;
    std::vector<double*> petalLength;
    std::vector<double*> petalWidth;
    for(auto i=0u;i<data.size();++i){
        sepalLength.emplace_back(&data[i][0]);
        sepalWidth.emplace_back(&data[i][1]);
        petalLength.emplace_back(&data[i][2]);
        petalWidth.emplace_back(&data[i][3]);
    }
    auto sepalLengthMax = *std::max_element(std::begin(sepalLength),std::end(sepalLength));
    auto sepalLengthMin = *std::min_element(std::begin(sepalLength),std::end(sepalLength));
    auto sepalWidthMax = *std::max_element(std::begin(sepalWidth),std::end(sepalWidth));
    auto sepalWidthMin = *std::min_element(std::begin(sepalWidth),std::end(sepalWidth));
    auto petalLengthMax = *std::max_element(std::begin(petalLength),std::end(petalLength));
    auto petalLengthMin = *std::min_element(std::begin(petalLength),std::end(petalLength));
    auto petalWidthMax = *std::max_element(std::begin(petalWidth),std::end(petalWidth));
    auto petalWidthMin = *std::min_element(std::begin(petalWidth),std::end(petalWidth));
    for(auto i=0u;i<data.size();++i){
        *sepalLength[i] = EvoAI::stableNormalize(*sepalLength[i],*sepalLengthMin,*sepalLengthMax);
        *sepalWidth[i] = EvoAI::stableNormalize(*sepalWidth[i],*sepalWidthMin,*sepalWidthMax);
        *petalLength[i] = EvoAI::stableNormalize(*petalLength[i],*petalLengthMin,*petalLengthMax);
        *petalWidth[i] = EvoAI::stableNormalize(*petalWidth[i],*petalWidthMin,*petalWidthMax);
    }
}

void evolveHyperneat(float errorThreehold, std::vector<std::vector<std::string>>& irisData){
    EvoAI::Population p(500,3,2,false, true);
    auto errorSum = 999.0;
    unsigned int gen = 0;
    while(errorSum > errorThreehold){
        p.orderGenomesByFitness();
        for(auto& g:p.getGenomes()){
            g->mutate();
            auto nn = std::make_unique<EvoAI::HyperNeat>(EvoAI::SubstrateInfo(4,1,8,3),*g,
                                                        EvoAI::HyperNeat::SubstrateConfiguration::GRID);
            auto trainingSets = createTrainingSets(irisData,0,irisData.size()/2);
            normalizeData(trainingSets.first);
            std::vector<std::vector<double>> results;
            for(auto i=0u;i<trainingSets.first.size();++i){
                nn->setInputs(std::move(trainingSets.first[i]));
                auto outputs = nn->run();
                results.push_back(std::move(outputs));
                nn->reset();
            }
            auto error = 0.0;
            for(auto i=0u;i<results.size();++i){
                error += std::fabs(trainingSets.second[i][0] - results[i][0]) +
                            std::fabs(trainingSets.second[i][1] - results[i][1]) +
                            std::fabs(trainingSets.second[i][2] - results[i][2]);
            }
            errorSum = error;
            g->setFitness(std::pow((results.size() * 3) - errorSum,2));
        }
        std::cout << "\rGeneration: " << gen << " avg: " << p.computeAvgFitness() << " Error: " << errorSum << " NumSpecies: " << p.getSpeciesSize() << "       ";
        std::flush(std::cout);
        if(errorSum > errorThreehold){
            p.reproduce(true,EvoAI::Population::SelectionType::TOURNAMENT);
        }else{
            std::cout << std::endl;
        }
        ++gen;
    }
    auto g = p.getBestGenome();
    g->writeToFile("irisClassGen.json");
    auto nn = std::make_unique<EvoAI::HyperNeat>(EvoAI::SubstrateInfo(4,1,8,3),*g,
                                                EvoAI::HyperNeat::SubstrateConfiguration::GRID);
    nn->writeToFile("irisClassNN.json");
    std::cout << "Creating Test dataSets..." << std::endl;
    auto testSets = createTrainingSets(irisData,irisData.size()/2,irisData.size());
    std::cout << "Processing Test Classification..." << std::endl;
    auto error = 0.0;
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
        if(nnOut != expectedOut){
            error += 1;
        }
        std::cout << nnOut << " : "<< expectedOut << std::endl;
        std::cout << "\tIris-setosa" << ":" << " - " << (outputs[0] * 100) << "%" << std::endl;
        std::cout << "\tIris-versicolor" << ":" << " - " << (outputs[1] * 100) << "%" << std::endl;
        std::cout << "\tIris-virginica" << ":" << " - " << (outputs[2] * 100) << "%" << std::endl;
        error /= testSets.second.size();
        error *= 100;
        auto accuracy = 100 - error;
        std::cout << "Error: " << error << "%" << " Accuracy: " << accuracy << "%" << std::endl;
    }
}
void evolveNeat(float errorThreehold, std::vector<std::vector<std::string>>& irisData){
    EvoAI::Population p(500,4,3);
    auto errorSum = 999.0;
    unsigned int gen = 0;
    while(errorSum > errorThreehold){
        p.orderGenomesByFitness();
        for(auto& g:p.getGenomes()){
            g->mutate();
            auto nn = EvoAI::Genome::makePhenotype(*g);
            auto trainingSets = createTrainingSets(irisData,0,irisData.size()/2);
            normalizeData(trainingSets.first);
            std::vector<std::vector<double>> results;
            for(auto i=0u;i<trainingSets.first.size();++i){
                nn->setInputs(std::move(trainingSets.first[i]));
                auto outputs = nn->run();
                results.push_back(std::move(outputs));
                nn->reset();
            }
            auto error = 0.0;
            for(auto i=0u;i<results.size();++i){
                error += std::fabs(trainingSets.second[i][0] - results[i][0]) +
                            std::fabs(trainingSets.second[i][1] - results[i][1]) +
                            std::fabs(trainingSets.second[i][2] - results[i][2]);
            }
            errorSum = error;
            g->setFitness(std::pow((results.size() * 3) - errorSum,2));
        }
        std::cout << "\rGeneration: " << gen << " avg: " << p.computeAvgFitness() << " Error: " << errorSum << " NumSpecies: " << p.getSpeciesSize() << "       ";
        std::flush(std::cout);
        if(errorSum > errorThreehold){
            p.reproduce(true,EvoAI::Population::SelectionType::TOURNAMENT);
        }else{
            std::cout << std::endl;
        }
        ++gen;
    }
    auto g = p.getBestGenome();
    g->writeToFile("irisClassGen.json");
    auto nn = EvoAI::Genome::makePhenotype(*g);
    nn->writeToFile("irisClassNN.json");
    std::cout << "Creating Test dataSets..." << std::endl;
    auto testSets = createTrainingSets(irisData,irisData.size()/2,irisData.size());
    std::cout << "Processing Test Classification..." << std::endl;
    auto error = 0.0;
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
        if(nnOut != expectedOut){
            error += 1;
        }
        std::cout << nnOut << " : "<< expectedOut << std::endl;
        std::cout << "\tIris-setosa" << ":" << " - " << (outputs[0] * 100) << "%" << std::endl;
        std::cout << "\tIris-versicolor" << ":" << " - " << (outputs[1] * 100) << "%" << std::endl;
        std::cout << "\tIris-virginica" << ":" << " - " << (outputs[2] * 100) << "%" << std::endl;
        error /= testSets.second.size();
        error *= 100;
        auto accuracy = 100 - error;
        std::cout << "Error: " << error << "%" << " Accuracy: " << accuracy << "%" << std::endl;
    }
}