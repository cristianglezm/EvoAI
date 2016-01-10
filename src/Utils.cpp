#include <EvoAI/Utils.hpp>

namespace EvoAI{
    double random(const double& min, const double& max){
        using Distribution = std::normal_distribution<double>;
        static std::random_device rd;
        static std::mt19937 rEngine(rd());
        static Distribution dice(min, max);
        return dice(rEngine);
    }
    std::unique_ptr<NeuralNetwork> createFeedForwardNN(const size_t& numInputs, const size_t& numHidden,
                                                         const size_t& numNeuronsPerHidden, const std::size_t& numOutputs,
                                                         const double& bias){
        std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>(numInputs,numHidden, numNeuronsPerHidden, numOutputs, bias);
        // input to hidden layer
        for(auto i=0u;i<numInputs;++i){
            for(auto j=0u;j<numNeuronsPerHidden;++j){
                nn->addConnection(Connection({0,i},{1,j}, random(-1.0,1.0)));
            }
        }
        //hidden layers to next hidden layer
        for(auto i=1u;i<numHidden;++i){
            for(auto j=0u;j<numNeuronsPerHidden;++j){
                for(auto z=0u;z<numNeuronsPerHidden;++z){
                    nn->addConnection(Connection({i,j},{i+1,z}, random(-1.0,1.0)));
                }
            }
        }
        // last hidden layer to output
        auto numLayers = nn->size();
        for(auto i=0u;i<numNeuronsPerHidden;++i){
            for(auto j=0u;j<numOutputs;++j){
                nn->addConnection(Connection({numLayers-2,i},{numLayers-1,j}, random(-1.0,1.0)));
            }
        }
        return std::move(nn);
    }
    std::unique_ptr<NeuralNetwork> createElmanNeuralNetwork(const std::size_t& numInputs, const std::size_t& numHidden, const std::size_t& numNeuronsPerHiddenLayer,
                                                                const std::size_t& numOutputs, const double& bias){
        std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>();
        nn->addLayer(NeuronLayer(numInputs,Neuron::Type::INPUT,bias));
        for(auto i=0u;i<numHidden;++i){
            nn->addLayer(NeuronLayer(numNeuronsPerHiddenLayer,Neuron::Type::CONTEXT,bias));
            nn->addLayer(NeuronLayer(numNeuronsPerHiddenLayer,Neuron::Type::HIDDEN,bias));
        }
        nn->addLayer(NeuronLayer(numOutputs,Neuron::Type::OUTPUT,bias));
        for(auto i=0u;i<numInputs;++i){
            for(auto j=0u;j<numNeuronsPerHiddenLayer;++j){
                nn->addConnection(Connection(Link(0,i),Link(2,j), random(-1.0,1.0)));
            }
        }
        auto numContext = numHidden * 2;
        for(auto i=2u;i<numContext;i+=2){
            for(auto j=0u;j<numNeuronsPerHiddenLayer;++j){
                // save values to Context
                nn->addConnection(Connection(Link(i,j),Link(i-1,j),1.0));
                for(auto k=0u;k<numNeuronsPerHiddenLayer;++k){
                    // connect forward
                    nn->addConnection(Connection(Link(i,j),Link(i+2,k), random(-1.0,1.0)));
                    // pass Values from Context backwards
                    nn->addConnection(Connection(Link(i-1,j),Link(i,k), random(-1.0,1.0)));
                }
            }
        }
        for(auto j=0u;j<numNeuronsPerHiddenLayer;++j){
            // save values to Context
            nn->addConnection(Connection(Link(numContext,j), Link(numContext-1,j),1.0));
            for(auto k=0u;k<numOutputs;++k){
                nn->addConnection(Connection(Link(numContext,j), Link(numContext+1,k), random(-1.0,1.0)));
            }
            for(auto i=0u;i<numNeuronsPerHiddenLayer;++i){
                // pass Values from Context
                nn->addConnection(Connection(Link(numContext-1,j),Link(numContext,i), random(-1.0,1.0)));
            }
        }
        return std::move(nn);
    }

    void generateImageFromCoordinates(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        sf::Image imgInput;
        if(!imgInput.loadFromFile(imageInput)){
            throw std::runtime_error(imageInput + " Couldn't be loaded");
        }
        generateImageFromCoordinates(imageInput, nn,imageOutput);
    }
    void generateImageFromCoordinates(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        if((*nn)[0].size() != 3){
            throw std::runtime_error("The neural Network needs to have 3 inputs");
        }
        if((*nn)[(*nn).size()-1].size() != 3){
            throw std::runtime_error("The neural Network needs to have 3 outputs");
        }
        int width = imageInput.getSize().x;
        int height = imageInput.getSize().y;
        sf::Image imgOutput;
        imgOutput.create(width, height);
        for(auto x=0;x<width;++x){
            for(auto y=0;y<height;++y){
                auto norm_x = (2*(x/width))-1;
                auto norm_y = (2*(y/height))-1;
                auto d = std::sqrt(((norm_x/2)^2) + ((norm_y /2)^2));
                std::vector<double> inputs;
                inputs.emplace_back((double)x);
                inputs.emplace_back((double)y);
                inputs.emplace_back((double)d);
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0] * 255,color[1] * 255,color[2] * 255));
            }
        }
        imgOutput.saveToFile(imageOutput);
    }
    void generateImageFromColor(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        sf::Image imgInput;
        if(!imgInput.loadFromFile(imageInput)){
            throw std::runtime_error(imageInput + " Couldn't be loaded");
        }
        generateImageFromColor(imageInput, nn,imageOutput);
    }
    void generateImageFromColor(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        if((*nn)[0].size() != 3){
            throw std::runtime_error("The neural Network needs to have 3 inputs");
        }
        if((*nn)[(*nn).size()-1].size() != 3){
            throw std::runtime_error("The neural Network needs to have 3 outputs");
        }
        int width = imageInput.getSize().x;
        int height = imageInput.getSize().y;
        sf::Image imgOutput;
        imgOutput.create(width, height);
        for(auto x=0;x<width;++x){
            for(auto y=0;y<height;++y){
                std::vector<double> inputs;
                auto imgColor = imageInput.getPixel(x,y);
                inputs.emplace_back((double)imgColor.r);
                inputs.emplace_back((double)imgColor.g);
                inputs.emplace_back((double)imgColor.b);
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0] * 255,color[1] * 255,color[2] * 255));
            }
        }
        imgOutput.saveToFile(imageOutput);
    }
    void generateImageFromColorAndCoordinates(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        sf::Image imgInput;
        if(!imgInput.loadFromFile(imageInput)){
            throw std::runtime_error(imageInput + " Couldn't be loaded");
        }
        generateImageFromColorAndCoordinates(imageInput, nn,imageOutput);
    }
    void generateImageFromColorAndCoordinates(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        if((*nn)[0].size() != 3){
            throw std::runtime_error("The neural Network needs to have 3 inputs");
        }
        if((*nn)[(*nn).size()-1].size() != 3){
            throw std::runtime_error("The neural Network needs to have 3 outputs");
        }
        int width = imageInput.getSize().x;
        int height = imageInput.getSize().y;
        sf::Image imgOutput;
        imgOutput.create(width, height);
        for(auto x=0;x<width;++x){
            for(auto y=0;y<height;++y){
                auto norm_x = (2*(x/width))-1;
                auto norm_y = (2*(y/height))-1;
                auto d = std::sqrt(((norm_x/2)^2) + ((norm_y /2)^2));
                std::vector<double> inputs;
                auto imgColor = imageInput.getPixel(x,y);
                inputs.emplace_back((double)imgColor.r);
                inputs.emplace_back((double)imgColor.g);
                inputs.emplace_back((double)imgColor.b);
                inputs.emplace_back((double)x);
                inputs.emplace_back((double)y);
                inputs.emplace_back((double)d);
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0] * 255,color[1] * 255,color[2] * 255));
            }
        }
        imgOutput.saveToFile(imageOutput);
    }
}