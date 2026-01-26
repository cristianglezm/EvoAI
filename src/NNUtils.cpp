#include <EvoAI/Utils/NNUtils.hpp>
#include <fstream>

namespace EvoAI{
    std::unique_ptr<NeuralNetwork> createFeedForwardNN(const size_t& numInputs, const size_t& numHidden,
                                                         const std::vector<size_t>& numNeuronsPerHidden, std::size_t numOutputs,
                                                         double bias){
        if(numHidden > 0 && numNeuronsPerHidden.size() < numHidden){
            throw std::out_of_range("createFeedForwardNN: numNeuronsPerHidden.size() < numHidden");
        }

        std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>(numInputs,numHidden, numNeuronsPerHidden, numOutputs, bias);
        auto initBiases = [&](NeuralNetwork& NN){
            for(auto i=0u;i<NN.size();++i){
                auto& l = NN[i];
                auto size = l.size();
                for(auto& n:l.getNeurons()){
                    n.setBiasWeight(randomGen().xavierInit(size, n.size()));
                }
            }
        };
        if(numHidden == 0u){
            // input to outputs
            for(auto i=0u;i<numInputs;++i){
                for(auto j=0u;j<numOutputs;++j){
                    nn->addConnection(Connection({0,i},{1,j}, randomGen().xavierInit(numInputs, numOutputs)));
                }
            }
            initBiases(*nn);
            return nn;
        }
        // input to first hidden layer
        for(auto i=0u;i<numInputs;++i){
            for(auto j=0u;j<numNeuronsPerHidden[0];++j){
                nn->addConnection(Connection({0,i},{1,j}, randomGen().xavierInit(numInputs, numNeuronsPerHidden[0])));
            }
        }
        // hidden layers to next hidden layer
        for(auto i=1u;i<numHidden;++i){
            for(auto j=0u;j<numNeuronsPerHidden[i-1];++j){
                for(auto z=0u;z<numNeuronsPerHidden[i];++z){
                    nn->addConnection(Connection({i,j},{i+1,z}, randomGen().xavierInit(numNeuronsPerHidden[i-1], numNeuronsPerHidden[i])));
                }
            }
        }
        // last hidden layer to output
        const auto lastHiddenSize = numNeuronsPerHidden.back();
        for(auto i=0u;i<lastHiddenSize;++i){
            for(auto j=0u;j<numOutputs;++j){
                nn->addConnection(Connection({nn->size()-2,i},{nn->size()-1,j}, randomGen().xavierInit(lastHiddenSize, numOutputs)));
            }
        }
        initBiases(*nn);
        return nn;
    }
    std::unique_ptr<NeuralNetwork> createElmanNeuralNetwork(
            std::size_t numInputs,
            std::size_t numHidden,
            const std::vector<std::size_t>& numNeuronsPerHiddenLayer,
            std::size_t numOutputs,
            double bias)
    {
        if(numHidden == 0u){
            throw std::out_of_range("Elman Neural Network needs to have 1 hidden layer or more");
        }

        std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>();

        // input layer
        nn->addLayer(NeuronLayer(numInputs, Neuron::Type::INPUT, bias));

        // context + hidden layers
        for(std::size_t h = 0; h < numHidden; ++h){
            nn->addLayer(NeuronLayer(numNeuronsPerHiddenLayer[h], Neuron::Type::CONTEXT, bias)); // layer = 1 + 2*h
            nn->addLayer(NeuronLayer(numNeuronsPerHiddenLayer[h], Neuron::Type::HIDDEN,  bias)); // layer = 2 + 2*h
        }

        // output layer
        nn->addLayer(NeuronLayer(numOutputs, Neuron::Type::OUTPUT, bias));

        // connect input -> first hidden
        for(std::size_t i = 0; i < numInputs; ++i){
            for(std::size_t j = 0; j < numNeuronsPerHiddenLayer[0]; ++j){
                nn->addConnection(Connection(
                    Link(0, i),
                    Link(2, j),
                    randomGen().xavierInit(numInputs, numNeuronsPerHiddenLayer[0])
                ));
            }
        }

        // connect hidden layers + context loops
        for(std::size_t h = 0; h < numHidden - 1; ++h){
            std::size_t ctx = 1 + 2*h;
            std::size_t hid = 2 + 2*h;
            std::size_t next_hid = 2 + 2*(h + 1);

            for(std::size_t j = 0; j < numNeuronsPerHiddenLayer[h]; ++j){

                // save hidden -> context
                nn->addConnection(Connection(Link(hid, j), Link(ctx, j), 1.0));

                // connect hidden(h) -> hidden(h+1)
                for(std::size_t z = 0; z < numNeuronsPerHiddenLayer[h+1]; ++z){
                    nn->addConnection(Connection(
                        Link(hid, j),
                        Link(next_hid, z),
                        randomGen().xavierInit(numNeuronsPerHiddenLayer[h], numNeuronsPerHiddenLayer[h+1])
                    ));
                }

                // pass context forward
                for(std::size_t k = 0; k < numNeuronsPerHiddenLayer[h]; ++k){
                    nn->addConnection(Connection(
                        Link(ctx, j),
                        Link(hid, k),
                        randomGen().xavierInit(numNeuronsPerHiddenLayer[h], numNeuronsPerHiddenLayer[h])
                    ));
                }
            }
        }

        // last hidden -> output + last context loop
        {
            std::size_t h = numHidden - 1;
            std::size_t ctx = 1 + 2*h;
            std::size_t hid = 2 + 2*h;
            std::size_t out = 2 + 2*numHidden;

            for(std::size_t j = 0; j < numNeuronsPerHiddenLayer[h]; ++j){

                // save hidden -> context
                nn->addConnection(Connection(Link(hid, j), Link(ctx, j), 1.0));

                // hidden -> output
                for(std::size_t k = 0; k < numOutputs; ++k){
                    nn->addConnection(Connection(
                        Link(hid, j),
                        Link(out, k),
                        randomGen().xavierInit(numNeuronsPerHiddenLayer[h], numOutputs)
                    ));
                }

                // context -> hidden
                for(std::size_t i = 0; i < numNeuronsPerHiddenLayer[h]; ++i){
                    nn->addConnection(Connection(
                        Link(ctx, j),
                        Link(hid, i),
                        randomGen().xavierInit(numNeuronsPerHiddenLayer[h], numNeuronsPerHiddenLayer[h])
                    ));
                }
            }
        }

        // initialize biases
        auto initBiases = [&](NeuralNetwork& NN){
            for(std::size_t i = 0; i < NN.size(); ++i){
                auto& l = NN[i];
                auto size = l.size();
                for(auto& n : l.getNeurons()){
                    n.setBiasWeight(randomGen().xavierInit(size, n.size()));
                }
            }
        };
        initBiases(*nn);
        return nn;
    }
    std::unique_ptr<NeuralNetwork> createFullyConnectedCPPN(std::size_t numInputs, std::size_t numHidden,
                                                            const std::vector<std::size_t>& numNeuronsPerHiddenLayer, std::size_t numOutputs, double bias){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        static std::default_random_engine g(seed);
        std::uniform_int_distribution<int> dice(0,Neuron::ActivationType::LAST_CPPN_ACTIVATION_TYPE-1);
        std::unique_ptr<NeuralNetwork> nn = createFeedForwardNN(numInputs,numHidden,numNeuronsPerHiddenLayer,numOutputs,bias);
        for(auto i=0u;i<nn->size();++i){
            for(auto j=0u;j<(*nn)[i].size();++j){
                (*nn)[i][j].setActivationType(static_cast<Neuron::ActivationType>(dice(g)));
            }
        }
        return nn;
    }
    std::unique_ptr<NeuralNetwork> createCPPN(std::size_t numInputs, std::size_t numHidden, const std::vector<std::size_t>& numNeuronsPerHiddenLayer, std::size_t numOutputs, double bias){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        static std::default_random_engine g(seed);
        std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>(numInputs,numHidden,numNeuronsPerHiddenLayer,numOutputs,bias);
        std::uniform_int_distribution<int> hiddenLayerDice(numHidden == 0 ? 0:1,numHidden);
        std::uniform_int_distribution<int> inputNeuronDice(0,numInputs-1);
        std::uniform_int_distribution<int> outputNeuronDice(0,numOutputs-1);
        std::bernoulli_distribution bernoulli(0.5); //recurrent connections
        auto outputLayer = numHidden+1;
        if(numHidden == 0){ // connect input to output
            for(auto i=0u;i<2u;++i){
                for(auto j=0u;j<4u;++j){
                    nn->addConnection(Connection(Link(0,inputNeuronDice(g)), 
                                                    Link(outputLayer,outputNeuronDice(g)), 
                                                    randomGen().xavierInit(numInputs, numOutputs)));
                    nn->addConnection(Connection(Link(0,inputNeuronDice(g)), 
                                                    Link(outputLayer,outputNeuronDice(g)), 
                                                    randomGen().xavierInit(numInputs, numOutputs)));
                    nn->addConnection(Connection(Link(0,inputNeuronDice(g)), 
                                                    Link(outputLayer,outputNeuronDice(g)), 
                                                    randomGen().xavierInit(numInputs, numOutputs)));
                }
                if(bernoulli(g)){
                    nn->addConnection(Connection(Link(0,inputNeuronDice(g)), 
                                                    Link(outputLayer,outputNeuronDice(g)), 
                                                    randomGen().xavierInit(numInputs, numOutputs)));
                    nn->addConnection(Connection(Link(outputLayer,outputNeuronDice(g)), 
                                                    Link(0,inputNeuronDice(g)), 
                                                    randomGen().xavierInit(numInputs, numOutputs)));
                }
            }
        }else{
            auto getRandomHiddenNeuron = [&](int max){
                return std::uniform_int_distribution<int>(0,max > 0 ? max-1:max)(g);
            };
            for(auto i=0u;i<2u;++i){
                for(auto j=0u;j<4u;++j){
                    auto l1 = hiddenLayerDice(g);
                    // connect input to hidden layer
                    nn->addConnection(Connection(Link(0,inputNeuronDice(g)),
                                                    Link(l1,getRandomHiddenNeuron(numNeuronsPerHiddenLayer[l1-1])),
                                                    randomGen().xavierInit(numInputs, numNeuronsPerHiddenLayer[l1-1])));
                    l1 = hiddenLayerDice(g);
                    auto l2 = hiddenLayerDice(g);
                    // connect hidden to hidden
                    nn->addConnection(Connection(Link(l1,getRandomHiddenNeuron(numNeuronsPerHiddenLayer[l1-1])),
                                                    Link(l2,getRandomHiddenNeuron(numNeuronsPerHiddenLayer[l2-1])),
                                                    randomGen().xavierInit(numInputs, numNeuronsPerHiddenLayer[l1-1])));
                    l1 = hiddenLayerDice(g);
                    // connect hidden to outputs
                    nn->addConnection(Connection(Link(l1,getRandomHiddenNeuron(numNeuronsPerHiddenLayer[l1-1])),
                                                    Link(numHidden+1,outputNeuronDice(g)),
                                                    randomGen().xavierInit(numNeuronsPerHiddenLayer[l1-1], numOutputs)));
                }
                if(bernoulli(g)){
                    for(auto z=0;z<2;++z){
                        auto hl1 = hiddenLayerDice(g);
                        auto hl2 = hiddenLayerDice(g);
                        nn->addConnection(Connection(Link(hl1,getRandomHiddenNeuron(numNeuronsPerHiddenLayer[hl1-1])),
                                                        Link(outputLayer,outputNeuronDice(g)),
                                                        randomGen().xavierInit(numNeuronsPerHiddenLayer[hl1-1], numOutputs)));
                        nn->addConnection(Connection(Link(outputLayer, outputNeuronDice(g)),
                                                        Link(hl2,getRandomHiddenNeuron(numNeuronsPerHiddenLayer[hl2-1])),
                                                        randomGen().xavierInit(numNeuronsPerHiddenLayer[hl2-1], numOutputs)));
                    }
                }
            }
        }
        std::uniform_int_distribution<int> dice(0,Neuron::ActivationType::LAST_CPPN_ACTIVATION_TYPE-1);
        for(auto i=0u;i<nn->size();++i){
            for(auto j=0u;j<(*nn)[i].size();++j){
                (*nn)[i][j].setActivationType(static_cast<Neuron::ActivationType>(dice(g)));
            }
        }
        return nn;
    }
    void UniformInit(NeuralNetwork& nn) noexcept{
        for(auto& l:nn.getLayers()){
            auto size = l.size();
            for(auto& n:l.getNeurons()){
                n.setBiasWeight(randomGen().random(-1.0, 1.0, size));
                for(auto& c:n.getConnections()){
                    c.setWeight(randomGen().random(-1.0, 1.0, size));
                }
            }
        }
    }
    void HeInit(NeuralNetwork& nn) noexcept{
        for(auto& l:nn.getLayers()){
            auto size = l.size();
            for(auto& n:l.getNeurons()){
                n.setBiasWeight(randomGen().heInit(size));
                for(auto& c:n.getConnections()){
                    c.setWeight(randomGen().heInit(size));
                }
            }
        }
    }
    void HeInitNormal(NeuralNetwork& nn) noexcept{
        for(auto& l:nn.getLayers()){
            auto size = l.size();
            for(auto& n:l.getNeurons()){
                n.setBiasWeight(randomGen().heInitNormal(size));
                for(auto& c:n.getConnections()){
                    c.setWeight(randomGen().heInitNormal(size));
                }
            }
        }
    }
    void XavierInit(NeuralNetwork& nn) noexcept{
        for(auto& l:nn.getLayers()){
            auto size = l.size();
            for(auto& n:l.getNeurons()){
                n.setBiasWeight(randomGen().xavierInit(size, n.size()));
                for(auto& c:n.getConnections()){
                    c.setWeight(randomGen().xavierInit(size, n.size()));
                }
            }
        }
    }
    void XavierInitNormal(NeuralNetwork& nn) noexcept{
        for(auto& l:nn.getLayers()){
            auto size = l.size();
            for(auto& n:l.getNeurons()){
                n.setBiasWeight(randomGen().xavierInitNormal(size, n.size()));
                for(auto& c:n.getConnections()){
                    c.setWeight(randomGen().xavierInitNormal(size, n.size()));
                }
            }
        }
    }
    void LeCunInit(NeuralNetwork& nn) noexcept{
        for(auto& l:nn.getLayers()){
            auto size = l.size();
            for(auto& n:l.getNeurons()){
                n.setBiasWeight(randomGen().lecunInit(size));
                for(auto& c:n.getConnections()){
                    c.setWeight(randomGen().lecunInit(size));
                }
            }
        }
    }
    void LeCunInitNormal(NeuralNetwork& nn) noexcept{
        for(auto& l:nn.getLayers()){
            auto size = l.size();
            for(auto& n:l.getNeurons()){
                n.setBiasWeight(randomGen().lecunInitNormal(size));
                for(auto& c:n.getConnections()){
                    c.setWeight(randomGen().lecunInitNormal(size));
                }
            }
        }
    }
    void writeMultiPlot(std::string_view filename, const std::vector<std::string>& legend, const std::vector<std::vector<double>>& data) noexcept{
        std::ofstream out(filename.data(), std::ios_base::out);
        for(auto i=0u;i<data.size();++i){
            out << legend[i] << ",";
            for(const auto& d:data[i]){
                auto commaOrSpace = &d != &data[i].back() ? ",":"";
                out << d << commaOrSpace;
            }
            out << "\n";
        }
        out.close();
    }
    std::size_t Argmax(const std::vector<double>& outputs) noexcept{
        return std::distance(outputs.begin(), std::max_element(outputs.begin(), outputs.end()));
    }
    std::size_t Argmax(NeuronLayer& outputs) noexcept{
        auto& neurons = outputs.getNeurons();
        return std::distance(neurons.begin(), std::max_element(neurons.begin(), neurons.end(),
            [](auto& n1,  auto& n2){
                return n1.getOutput() < n2.getOutput();
        }));
    }
}
