#include <iostream>
#include <EvoAI.hpp>
#include <utility>
#include <memory>
#include <vector>
#include <fstream>

void usage();
class IrisDataset;
/**
 * @brief Evolves a population of EvoAI::Genome to classify the Iris Dataset using NEAT.
 * @param lossThreshold float when to stop loss lower than loss Threshold
 * @param trainingSet EvoAI::DataLoader<IrisDataset>&
 * @param testingSet EvoAI::DataLoader<IrisDataset>&
 */
void evolveNeat(float lossThreshold, EvoAI::DataLoader<IrisDataset>& trainingSet, EvoAI::DataLoader<IrisDataset>& testingSet, bool normalize) noexcept;
/**
 * @brief Evolves a population of EvoAI::Genome to classify the Iris Dataset using HyperNEAT.
 * @param lossThreshold float when to stop loss lower than loss Threshold
 * @param trainingSet EvoAI::DataLoader<IrisDataset>&
 * @param testingSet EvoAI::DataLoader<IrisDataset>&
 */
void evolveHyperneat(float lossThreshold, EvoAI::DataLoader<IrisDataset>& trainingSet, EvoAI::DataLoader<IrisDataset>& testingSet, bool normalize) noexcept;
/// @brief Scalers for the data of the iris dataset to be applied to columns 0 to 3
using Scalers = std::array<EvoAI::Scaler<double>, 4>;
/**
 * @brief normalizes the data.
 * @details Scalers
 *      0. sepal length in cm
 *      1. sepal width in cm
 *      2. petal length in cm
 *      3. petal width in cm
 * @param data std::vector<std::pair<std::vector<double>,std::vector<double>>>&
 * @param scalers Scalers&
 */
void normalizeData(std::vector<std::pair<std::vector<double>,std::vector<double>>>& data, Scalers& scalers);
/**
 * @brief Iris Dataset
 */
struct IrisDataset{
    using TrainingFormat = std::vector<std::pair<std::vector<double>,std::vector<double>>>;
    IrisDataset(std::string_view filename, bool train, std::size_t batchSize)
    : m_index(0u)
    , m_batchSize(batchSize){
        std::string dataInput(filename);
        std::fstream csv(dataInput);
        auto irisData = EvoAI::readCSVFile(csv);
        csv.close();
        std::mt19937_64 g = EvoAI::randomGen().getEngine();
        std::shuffle(std::begin(irisData), std::end(irisData), g);
        std::size_t start = 0u;
        auto percent = 1.0;
        if(train){
            percent = 0.8;
        }else{
            percent = 0.2;
        }
        std::size_t end = std::floor(irisData.size() * percent);
        for(auto i=start;i<end;++i){
            std::vector<double> in;
            in.reserve(irisData[i].size());
            std::vector<double> out;
            out.reserve(3);
            auto size = irisData[i].size()-1;
            for(auto j=0u;j<size;++j){
                in.emplace_back(std::stod(irisData[i][j]));
            }
            if(irisData[i][size] == "Iris-setosa"){
                out.emplace_back(1.0);
                out.emplace_back(0.0);
                out.emplace_back(0.0);
            }else if(irisData[i][size] == "Iris-versicolor"){
                out.emplace_back(0.0);
                out.emplace_back(1.0);
                out.emplace_back(0.0);
            }else if(irisData[i][size] == "Iris-virginica"){
                out.emplace_back(0.0);
                out.emplace_back(0.0);
                out.emplace_back(1.0);
            }
            m_data.emplace_back(in, out);
        }
    }
    const std::pair<std::vector<double>&, std::vector<double>&> operator()() noexcept{
        auto i = m_index;
        m_index = (m_index + 1) % m_data.size();
        return std::make_pair(std::ref(m_data[i].first), std::ref(m_data[i].second));
    }
    std::size_t size() const noexcept{
        return (m_data.size() + m_batchSize - 1) / m_batchSize;
    }
    std::size_t getBatchSize() const noexcept{
        return m_batchSize;
    }
    template<class Fn>
    auto transform(Fn&& fn) noexcept{
        return fn(m_data);
    }
    template<class Fn, class...Args>
    auto transform(Fn&& fn, Args&...args) noexcept{
        return fn(m_data, args...);
    }
    void shuffle() noexcept{
        auto g = EvoAI::randomGen().getEngine();
        std::uniform_int_distribution ud(0, static_cast<int>(m_data.size() - 1));
        for(auto i=0u;i<m_data.size();++i){
            auto index1 = ud(g);
            auto index2 = ud(g);
            std::swap(m_data[index1], m_data[index2]);
        }
    }
    // data
    mutable std::size_t m_index;
    std::size_t m_batchSize;
    std::vector<std::pair<std::vector<double>,std::vector<double>>> m_data;
};
/**
 * @brief Test Fn to pass to EvoAI::NeuralNetwork::train
 * @param nn EvoAI::NeuralNetwork& 
 * @param ids EvoAI::DataLoader<IrisDataset>&
 * @return std::pair<double, double> avgTestLoss and accuracy
 */
std::pair<double, double> testDataset(EvoAI::NeuralNetwork& nn, EvoAI::DataLoader<IrisDataset>& ids) noexcept;
/**
 * @brief Model
 */
class IrisClassifier{
    public:
        IrisClassifier(bool normalize = false, std::unique_ptr<EvoAI::NeuralNetwork>&& net = nullptr) noexcept
        : m_nn{initHelper(std::forward<std::unique_ptr<EvoAI::NeuralNetwork>>(net))}
        , m_scalers()
        , m_normalize(normalize){
            if(m_normalize){
                for(auto i=0;i<4;++i){
                    m_scalers[i].normMin = 0.0;
                    m_scalers[i].normMax = 1.0;
                }
                /*
                    Summary Statistics:
                                  Min  Max   Mean    SD   Class    Correlation
                    sepal length: 4.3  7.9   5.84  0.83    0.7826   
                    sepal width:  2.0  4.4   3.05  0.43   -0.4194
                    petal length: 1.0  6.9   3.76  1.76    0.9490  (high!)
                    petal width:  0.1  2.5   1.20  0.76    0.9565  (high!)
                */
                m_scalers[0].min = 4.3;
                m_scalers[0].max = 7.9;

                m_scalers[1].min = 2.0;
                m_scalers[1].max = 4.4;

                m_scalers[2].min = 1.0;
                m_scalers[2].max = 6.9;

                m_scalers[3].min = 0.1;
                m_scalers[3].max = 2.5;
            }
        }
        IrisClassifier(std::string_view filename) noexcept
        : m_nn{nullptr}
        , m_scalers()
        , m_normalize(false){
            JsonBox::Value v;
            v.loadFromFile(filename.data());
            m_normalize = v["IrisClassifier"]["normalize"].getBoolean();
            m_nn = std::make_unique<EvoAI::NeuralNetwork>(v["IrisClassifier"]["nn"].getObject());
            m_scalers[0] = EvoAI::Scaler<double>(v["IrisClassifier"]["SepalLength"].getObject());
            m_scalers[1] = EvoAI::Scaler<double>(v["IrisClassifier"]["SepalWidth"].getObject());
            m_scalers[2] = EvoAI::Scaler<double>(v["IrisClassifier"]["PetalLength"].getObject());
            m_scalers[3] = EvoAI::Scaler<double>(v["IrisClassifier"]["PetalWidth"].getObject());
        }
        std::vector<double> forward(const std::vector<double>& inputs) noexcept{
            if(m_normalize){
                std::vector<double> normInputs(4, 0.0);
                normInputs[0] = m_scalers[0].transform(inputs[0]);
                normInputs[1] = m_scalers[1].transform(inputs[1]);
                normInputs[2] = m_scalers[2].transform(inputs[2]);
                normInputs[3] = m_scalers[3].transform(inputs[3]);
                auto out =  m_nn->forward(normInputs);
                m_nn->reset();
                return out;
            }else{
                auto out = m_nn->forward(inputs);
                m_nn->reset();
                return out;
            }
        }
        void train(EvoAI::DataLoader<IrisDataset>& trainingSet, EvoAI::DataLoader<IrisDataset>& testingSet, std::size_t epoch, std::size_t batchSize) noexcept{
            if(m_normalize){
                trainingSet.getDataset().transform(&normalizeData, m_scalers);
                testingSet.getDataset().transform(&normalizeData, m_scalers);
            }
            EvoAI::Optimizer optim(0.1, batchSize, EvoAI::SGD(m_nn->getParameters(), 0.0), EvoAI::Scheduler(EvoAI::MultiStepLR({175}, 0.1)));
            EvoAI::writeMultiPlot("irisAvgLoss.txt", {"epochAvgLoss", "testAvgLoss", "accuracy"},
                m_nn->train(trainingSet, testingSet, optim, epoch, EvoAI::Loss::MultiClassCrossEntropy{}, testDataset));
        }
        void test(EvoAI::DataLoader<IrisDataset>& ids) noexcept{
            if(m_normalize){
                ids.getDataset().transform(&normalizeData, m_scalers);
            }
            testDataset(*m_nn, ids);
        }
        JsonBox::Value toJson() const noexcept{
            auto v = EvoAI::makeJsonFrom({"nn", "SepalLength", "SepalWidth", "PetalLength", "PetalWidth"}, 
                                    *m_nn, m_scalers[0], m_scalers[1], m_scalers[2], m_scalers[3]);
            v["normalize"] = m_normalize;
            return v;
        }
        void writeToFile(std::string_view filename) noexcept{
            JsonBox::Value v;
            v["version"] = 1.0;
            v["IrisClassifier"] = toJson();
            v.writeToFile(filename.data(), false, false);
        }
        void writeDotFile(std::string_view filaname) noexcept{
            m_nn->writeDotFile(filaname.data());
        }
        Scalers& getScalers() noexcept{
            return m_scalers;
        }
    private:
        std::unique_ptr<EvoAI::NeuralNetwork> initHelper(std::unique_ptr<EvoAI::NeuralNetwork>&& network) noexcept{
            auto net = std::move(network);
            if(!net){
                net = EvoAI::createFeedForwardNN(4, 2, {128,64}, 3, 1.0);
                EvoAI::UniformInit(*net);
                (*m_nn)[1].setActivationType(EvoAI::Neuron::ActivationType::RELU);
                (*m_nn)[2].setActivationType(EvoAI::Neuron::ActivationType::RELU);
                (*m_nn)[3].setActivationType(EvoAI::Neuron::ActivationType::SOFTMAX);
            }
            return net;
        }
    private:
        std::unique_ptr<EvoAI::NeuralNetwork> m_nn;
        std::array<EvoAI::Scaler<double>, 4> m_scalers;
        bool m_normalize;
};

struct Options{
    std::string dataFilename{""};
    bool train = false;
    bool classify = false;
    bool normalize = false;
    bool evolve = false;
    std::string modelFilename{""};
    float lossThreshold = 0.07;
    bool hyperneat = false;
};

Options parseArgs(int argc, char** argv) noexcept{
    Options options;
    for(auto i=0;i<argc;++i){
        std::string opt(argv[i]);
        if(opt == "-t" || opt == "--train"){
            options.train = true;
        }else if(opt == "-df"){
            options.dataFilename = std::string(argv[i+1]);
        }else if(opt == "-n" || opt == "--norm"){
            options.normalize = true;
        }else if(opt == "-e" || opt == "--evolve"){
            options.evolve = true;
            options.lossThreshold = std::stof(std::string(argv[i+1]));
            std::string hn = "";
            if(argv[i+2] != nullptr){
                hn = std::string(argv[i+2]);
                if(hn == "hn"){
                    options.hyperneat = true;
                }
            }
        }else if(opt == "-c" || opt == "--classify"){
            options.classify = true;
        }else if(opt == "-m" || opt == "--model"){
            auto nextArg = std::string(argv[i+1]);
            if(!nextArg.empty()){
                options.modelFilename = nextArg;
            }
        }else if(opt == "-h" || opt == "--help"){
            usage();
        }
    }
    return options;
}
int main(int argc, char **argv){
    EvoAI::randomGen().setSeed(42);
    std::cout << "initializing seed to " << EvoAI::randomGen().getSeed() << std::endl;
    if(argc < 3){
        usage();
        return EXIT_FAILURE;
    }
    Options options = parseArgs(argc, argv);
    if(options.dataFilename.empty()){
        std::cout << "error: not datafile given" << std::endl;
        return EXIT_FAILURE;
    }
    std::unique_ptr<IrisClassifier> irisClass = nullptr;
    if(options.train){
        if(!options.modelFilename.empty()){
            std::cout << "loading model from " << options.modelFilename << std::endl;
            irisClass = std::make_unique<IrisClassifier>(options.modelFilename);
        }else{
            std::cout << "initializing new model" << std::endl;
            irisClass = std::make_unique<IrisClassifier>(options.normalize);
        }
        std::cout << "Training Iris Classifier..." << std::endl;
        auto batchSize = 64;
        auto epoch = 380;
        if(options.normalize){
            epoch = 90;
        }
        EvoAI::DataLoader<IrisDataset> trainingSet(IrisDataset(options.dataFilename, true, batchSize));
        EvoAI::DataLoader<IrisDataset> testingSet(IrisDataset(options.dataFilename, false, batchSize));
        std::cout << "training for " << epoch << " epochs " << std::endl;
        irisClass->train(trainingSet, testingSet, epoch, batchSize);
        std::cout << "writing new model to IrisModel.json" << std::endl;
        irisClass->writeToFile("irisModel.json");
        std::cout << "writing dot file for new model to IrisModel.dot" << std::endl;
        irisClass->writeDotFile("IrisModel.dot");
    }else if(options.evolve){
        std::cout << "It will stop when loss is less than " << options.lossThreshold << std::endl;
        auto batchSize = 64;
        EvoAI::DataLoader<IrisDataset> trainingSet(IrisDataset(options.dataFilename, true, batchSize));
        EvoAI::DataLoader<IrisDataset> testingSet(IrisDataset(options.dataFilename, false, batchSize));
        if(options.hyperneat){
            std::cout << "Evolving Population using HyperNEAT" << std::endl;
            evolveHyperneat(options.lossThreshold, trainingSet, testingSet, options.normalize);
        }else{
            std::cout << "Evolving Population using NEAT" << std::endl;
            evolveNeat(options.lossThreshold, trainingSet, testingSet, options.normalize);
        }
    }else if(options.classify){
        if(!options.modelFilename.empty()){
            std::cout << "loading model from " << options.modelFilename << std::endl;
            irisClass = std::make_unique<IrisClassifier>(options.modelFilename);
        }else{
            std::cout << "initializing new model" << std::endl;
            irisClass = std::make_unique<IrisClassifier>(options.normalize);
        }
        EvoAI::DataLoader<IrisDataset> testingSet(IrisDataset(options.dataFilename, false, 64));
        irisClass->test(testingSet);
    }
    return 0;
}
void usage(){
    std::cout << "usage: IrisClassification -df <dataInput> <options> [-m <filename>]\n";
    std::cout << "examples:\n";
    std::cout << "IrisClassification -df iris.data.csv -t // to train from zero\n";
    std::cout << "IrisClassification -df iris.data.csv -t -m irisModel.json // to keep training irisModel.json\n";
    std::cout << "V evolve a population till loss is under 0.07 if hn HyperNeat will be used.\n";
    std::cout << "IrisClassification -df iris.data.csv -e 0.07 [hn]\n";
    std::cout << "\t-df <dataFilename>\t\t\tThe file with the Iris Dataset.\n";
    std::cout << "\tdataInput is a csv file with this format <sepal length>,<sepal width>,<petal length>,<petal width>,Iris-setosa || Iris-versicolor || Iris-virginica\n";
    std::cout << "\t-m, --model <filename>\t\t\tWill load a IrisClassifier Model\n";
    std::cout << "\t\tfilename should be a json file of an IrisClassifier if empty it will create a random nn and save it as irisModel.json to train or test.\n";
    std::cout << "\t-n, --norm\t\t\t\tWill normalize features of the data\n";
    std::cout << "\t-t, --train\t\t\t\tWill train the network specified.\n";
    std::cout << "\t-c, --classify\t\t\t\tWill test the network with the test data.\n";
    std::cout << "\t-e, --evolve <lossThreshold> [hn]\tWill evolve a population and select the best.\n";
    std::cout << "\t\t\t\t\t\t[hn] will use hyperneat instead of neat\n";
    std::cout << "\t-h, --help\t\t\t\tthis menu." << std::endl;
}
void normalizeData(std::vector<std::pair<std::vector<double>,std::vector<double>>>& data, std::array<EvoAI::Scaler<double>, 4>& scalers){
    for(auto i=0u;i<data.size();++i){
        data[i].first[0u] = scalers[0u].transform(data[i].first[0u]);
        data[i].first[1u] = scalers[1u].transform(data[i].first[1u]);
        data[i].first[2u] = scalers[2u].transform(data[i].first[2u]);
        data[i].first[3u] = scalers[3u].transform(data[i].first[3u]);
    }
}
std::pair<double, double> testDataset(EvoAI::NeuralNetwork& nn, EvoAI::DataLoader<IrisDataset>& ids) noexcept{
    auto correct = 0u;
    auto samples = ids.size();
    auto batchSize = ids.getBatchSize();
    auto totalAvgLoss = 0.0;
    std::array<std::string, 3> classes = {"Iris-setosa","Iris-versicolor","Iris-virginica"};
    for(auto i=0u;i<samples;++i){
        for(auto j=0u;j<batchSize;++j){
            auto [inputs, target] = ids();
            auto outputs = nn.forward(inputs);
            nn.reset();
            totalAvgLoss += EvoAI::Loss::MultiClassCrossEntropy{}(target, outputs);
            auto outStr = classes[EvoAI::Argmax(outputs)];
            auto targetStr = classes[EvoAI::Argmax(target)];
            auto isCorrect = EvoAI::Argmax(outputs) == EvoAI::Argmax(target);
            if(isCorrect){
                ++correct;
            }
            std::cout << outStr << " : "<< targetStr << "\t\t" << (outStr == targetStr ? "v/\n":"X\n");
            std::cout << "\tIris-setosa" << ":" << " - " << (outputs[0] * 100) << "%\n";
            std::cout << "\tIris-versicolor" << ":" << " - " << (outputs[1] * 100) << "%\n";
            std::cout << "\tIris-virginica" << ":" << " - " << (outputs[2] * 100) << "%" << std::endl;
        }
    }
    double accuracy = 100 * (correct / static_cast<double>(samples * batchSize));
    double testAvgLoss = totalAvgLoss / (samples * batchSize);
    std::cout << "Accuracy: " << accuracy << "% testAvgLoss: " << testAvgLoss << std::endl;
    return std::make_pair(testAvgLoss, accuracy);
}
void evolveHyperneat(float lossThreshold, EvoAI::DataLoader<IrisDataset>& trainingSet, EvoAI::DataLoader<IrisDataset>& testingSet, bool normalize) noexcept{
    /// @warning important for the population to be cppn for HyperNEAT
    EvoAI::Population<EvoAI::Genome> p(50,2.0, 2.0, 1.0, 3,2,false, true);
    p.setCompatibilityThreshold(10.0);
    auto loss = 999.0;
    std::size_t gen = 0u;
    auto makeIrisClass = [&normalize](EvoAI::Genome& g){
        auto nn = std::make_unique<EvoAI::HyperNeat>(EvoAI::SubstrateInfo(4,2,{64, 32},3),g,
                                                    EvoAI::HyperNeat::SubstrateConfiguration::GRID);
        nn->getSubstrate()[1].setActivationType(EvoAI::Neuron::ActivationType::RELU);
        nn->getSubstrate()[2].setActivationType(EvoAI::Neuron::ActivationType::RELU);
        nn->getSubstrate()[3].setActivationType(EvoAI::Neuron::ActivationType::SOFTMAX);
        return std::make_unique<IrisClassifier>(normalize, std::make_unique<EvoAI::NeuralNetwork>(nn->getSubstrate()));
    };
    auto eval = [&](auto& g){
        g.mutate();
        auto irisClass = makeIrisClass(g);
        auto size = trainingSet.size() * trainingSet.getBatchSize();
        loss = 0.0;
        for(auto i=0u;i<size;++i){
            auto [input, target] = trainingSet();
            auto outputs = irisClass->forward(input);
            loss += EvoAI::Loss::MultiClassCrossEntropy{}(target, outputs);
        }
        loss /= size;
        g.setFitness(100.0 - loss);
    };
    while(loss >= lossThreshold){
        p.eval(eval);
        std::cout << "\rGeneration: " << gen << " avg fitness: " << p.computeAvgFitness() << " Loss: " << loss << " NumSpecies: " << p.getSpeciesSize() << "       ";
        std::flush(std::cout);
        if(loss >= lossThreshold){
            p.reproduce(EvoAI::SelectionAlgorithms::Tournament<EvoAI::Genome>{p.getPopulationMaxSize(), 5}, true);
            p.increaseAgeAndRemoveOldSpecies();
            p.regrowPopulation(2.0, 2.0, 1.0, 3, 2);
        }else{
            std::cout << std::endl;
        }
        ++gen;
    }
    auto g = p.getBestMember();
    g->writeToFile("irisGenHN.json");
    auto irisClass = makeIrisClass(*g);
    irisClass->writeToFile("IrisHN.json");
    irisClass->writeDotFile("IrisHN.dot");
    irisClass->test(testingSet);
}
void evolveNeat(float lossThreshold, EvoAI::DataLoader<IrisDataset>& trainingSet, EvoAI::DataLoader<IrisDataset>& testingSet, bool normalize) noexcept{
    EvoAI::Population<EvoAI::Genome> p(500, 2.0, 2.0, 1.0, 4,3);
    p.setCompatibilityThreshold(10.0);
    auto loss = 999.0;
    std::size_t gen = 0u;
    auto makeIrisClass = [&normalize](EvoAI::Genome& g){
        auto nn = EvoAI::Genome::makePhenotype(g);
        nn[1].setActivationType(EvoAI::Neuron::ActivationType::RELU);
        nn[2].setActivationType(EvoAI::Neuron::ActivationType::SOFTMAX);
        return std::make_unique<IrisClassifier>(normalize, std::make_unique<EvoAI::NeuralNetwork>(std::move(nn)));
    };
    auto eval = [&](auto& g){
        loss = 0.0;
        g.mutate();
        auto irisClass = makeIrisClass(g);
        auto size = trainingSet.size() * trainingSet.getBatchSize();
        for(auto i=0u;i<size;++i){
            auto [input, target] = trainingSet();
            auto outputs = irisClass->forward(input);
            loss += EvoAI::Loss::MultiClassCrossEntropy{}(target, outputs);
        }
        loss /= size;
        g.setFitness(100.0 - loss);
    };
    while(loss > lossThreshold){
        p.eval(eval);
        std::cout << "\rGeneration: " << gen << " avg fitness: " << p.computeAvgFitness() << " NumSpecies: " << p.getSpeciesSize() << " Loss: " << loss << "       ";
        std::flush(std::cout);
        if(loss > lossThreshold){
            p.reproduce(EvoAI::SelectionAlgorithms::Tournament<EvoAI::Genome>{p.getPopulationMaxSize(), 5}, true);
            p.increaseAgeAndRemoveOldSpecies();
            p.regrowPopulation(2.0, 2.0, 1.0, 4, 3);
        }else{
            std::cout << std::endl;
        }
        ++gen;
    }
    auto g = p.getBestMember();
    g->writeToFile("irisGenNEAT.json");
    auto irisClass = makeIrisClass(*g);
    irisClass->writeToFile("irisNEAT.json");
    irisClass->writeDotFile("IrisNEAT.dot");
    irisClass->test(testingSet);
}
