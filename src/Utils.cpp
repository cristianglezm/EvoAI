#include <EvoAI/Utils.hpp>

namespace EvoAI{
    double random(const double& min, const double& max){
        using Distribution = std::uniform_real_distribution<double>;
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        //std::random_device{}();
        static std::default_random_engine rEngine(seed);
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
    std::unique_ptr<NeuralNetwork> createFullyConnectedCPPN(const std::size_t& numInputs, const std::size_t& numHidden,
                                                            const std::size_t& numNeuronsPerHiddenLayer, const std::size_t& numOutputs, const double& bias){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        static std::default_random_engine g(seed);
        static std::uniform_int_distribution<int> dice(0,Neuron::ActivationType::LAST_ACTIVATION_TYPE-1);
        std::unique_ptr<NeuralNetwork> nn = createFeedForwardNN(numInputs,numHidden,numNeuronsPerHiddenLayer,numOutputs,bias);
        for(auto i=0u;i<nn->size();++i){
            for(auto j=0u;j<(*nn)[i].size();++j){
                (*nn)[i][j].setActivationType(static_cast<Neuron::ActivationType>(dice(g)));
            }
        }
        return std::move(nn);
    }
    std::unique_ptr<NeuralNetwork> createCPPN(const std::size_t& numInputs, const std::size_t& numHidden, const std::size_t& numNeuronsPerHiddenLayer, const std::size_t& numOutputs, const double& bias){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        static std::default_random_engine g(seed);
        static std::uniform_int_distribution<int> dice(0,Neuron::ActivationType::LAST_ACTIVATION_TYPE-1);
        std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>(numInputs,numHidden,numNeuronsPerHiddenLayer,numOutputs,bias);
        static std::uniform_int_distribution<int> hiddenLayerDice(1,numHidden);
        static std::uniform_int_distribution<int> outputLayerDice(numHidden,numHidden+1);
        static std::uniform_int_distribution<int> inputNeuronDice(0,numInputs-1);
        static std::uniform_int_distribution<int> hiddenNeuronDice(0,numNeuronsPerHiddenLayer-1);
        static std::uniform_int_distribution<int> outputNeuronDice(0,numOutputs-1);
        static std::bernoulli_distribution bernoulli(0.5);
        for(auto i=0u;i<nn->size();++i){
            nn->addConnection(Connection(Link(0,inputNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(0,inputNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(0,inputNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(0,inputNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            
            nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
            
            nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(numHidden+1,outputNeuronDice(g)),random(-1.0,1.0)));
            nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(numHidden+1,outputNeuronDice(g)),random(-1.0,1.0)));
            if(bernoulli(g)){
                nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(outputLayerDice(g),outputNeuronDice(g)),random(-1.0,1.0)));
                nn->addConnection(Connection(Link(hiddenLayerDice(g),hiddenNeuronDice(g)),Link(outputLayerDice(g),outputNeuronDice(g)),random(-1.0,1.0)));
                
                nn->addConnection(Connection(Link(outputLayerDice(g),outputNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));
                nn->addConnection(Connection(Link(outputLayerDice(g),outputNeuronDice(g)),Link(hiddenLayerDice(g),hiddenNeuronDice(g)),random(-1.0,1.0)));                
            }
        }
        for(auto i=0u;i<nn->size();++i){
            for(auto j=0u;j<(*nn)[i].size();++j){
                (*nn)[i][j].setActivationType(static_cast<Neuron::ActivationType>(dice(g)));
            }
        }
        return std::move(nn);
    }
    void generateImageFromCoordinates(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        sf::Image imgInput;
        if(!imgInput.loadFromFile(imageInput)){
            throw std::runtime_error(imageInput + " Couldn't be loaded");
        }
        generateImageFromCoordinates(imgInput, nn,imageOutput);
    }
    void generateImageFromCoordinates(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        if((*nn)[0].size() != 3){
            throw std::runtime_error("EvoAI::generateImageFromCoordinates() : The neural Network needs to have 3 inputs");
        }
        if((*nn)[(*nn).size()-1].size() != 3){
            throw std::runtime_error("EvoAI::generateImageFromCoordinates() : The neural Network needs to have 3 outputs");
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
                inputs.emplace_back((double)x-(width/2));
                inputs.emplace_back((double)y-(height/2));
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
        generateImageFromColor(imgInput, nn,imageOutput);
    }
    void generateImageFromColor(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        if((*nn)[0].size() != 3){
            throw std::runtime_error("EVoAI::generateImageFromColor() : The neural Network needs to have 3 inputs");
        }
        if((*nn)[(*nn).size()-1].size() != 3){
            throw std::runtime_error("EvoAI::generateImageFromColor() : The neural Network needs to have 3 outputs");
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
    void generateBWImageFromColor(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        sf::Image imgInput;
        if(!imgInput.loadFromFile(imageInput)){
            throw std::runtime_error(imageInput + " Couldn't be loaded");
        }
        generateBWImageFromColor(imgInput, nn,imageOutput);
    }
    void generateBWImageFromColor(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        if((*nn)[0].size() != 3){
            throw std::runtime_error("EvoAI::generateBWImageFromColor() : The neural Network needs to have 3 inputs");
        }
        if((*nn)[(*nn).size()-1].size() != 1){
            throw std::runtime_error("EvoAI::generateBWImageFromColor() : The neural Network needs to have 1 outputs");
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
                imgOutput.setPixel(x,y,sf::Color(color[0] * 255,color[0] * 255,color[0] * 255));
            }
        }
        imgOutput.saveToFile(imageOutput);
    }
    void generateBWImageFromCoords(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        sf::Image imgInput;
        if(!imgInput.loadFromFile(imageInput)){
            throw std::runtime_error(imageInput + " Couldn't be loaded");
        }
        generateBWImageFromCoords(imgInput, nn,imageOutput);
    }
    void generateBWImageFromCoords(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        if((*nn)[0].size() != 3){
            throw std::runtime_error("EvoAI::generateBWImageFromCoords() : The neural Network needs to have 3 inputs");
        }
        if((*nn)[(*nn).size()-1].size() != 1){
            throw std::runtime_error("EvoAI::generateBWImageFromCoords() : The neural Network needs to have 1 outputs");
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
                inputs.emplace_back((double)x-(width/2));
                inputs.emplace_back((double)y-(height/2));
                inputs.emplace_back((double)d);
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0] * 255,color[0] * 255,color[0] * 255));
            }
        }
        imgOutput.saveToFile(imageOutput);
    }
    void generateBWImageFromColorAndCoordinates(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        sf::Image imgInput;
        if(!imgInput.loadFromFile(imageInput)){
            throw std::runtime_error(imageInput + " Couldn't be loaded");
        }
        generateBWImageFromColorAndCoordinates(imgInput, nn,imageOutput);
    }
    void generateBWImageFromColorAndCoordinates(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        if((*nn)[0].size() != 6){
            throw std::runtime_error("EvoAI::generateImageFromColorAndCoordinates() : The neural Network needs to have 6 inputs");
        }
        if((*nn)[(*nn).size()-1].size() != 1){
            throw std::runtime_error("EvoAI::generateImageFromColorAndCoordinates() : The neural Network needs to have 1 outputs");
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
                inputs.emplace_back((double)x-(width/2));
                inputs.emplace_back((double)y-(height/2));
                inputs.emplace_back((double)d);
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0] * 255,color[0] * 255,color[0] * 255));
            }
        }
        imgOutput.saveToFile(imageOutput);
    }
    void generateImageFromColorAndCoordinates(const std::string& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        sf::Image imgInput;
        if(!imgInput.loadFromFile(imageInput)){
            throw std::runtime_error(imageInput + " Couldn't be loaded");
        }
        generateImageFromColorAndCoordinates(imgInput, nn,imageOutput);
    }
    void generateImageFromColorAndCoordinates(sf::Image& imageInput, NeuralNetwork* nn, const std::string& imageOutput){
        if((*nn)[0].size() != 6){
            throw std::runtime_error("EvoAI::generateImageFromColorAndCoordinates() : The neural Network needs to have 6 inputs");
        }
        if((*nn)[(*nn).size()-1].size() != 3){
            throw std::runtime_error("EvoAI::generateImageFromColorAndCoordinates() : The neural Network needs to have 3 outputs");
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
                inputs.emplace_back((double)x-(width/2));
                inputs.emplace_back((double)y-(height/2));
                inputs.emplace_back((double)d);
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0] * 255,color[1] * 255,color[2] * 255));
            }
        }
        imgOutput.saveToFile(imageOutput);
    }
    void generateSoundFromCoordinates(const int& width, const int& height, NeuralNetwork* nn, const std::string& soundOutput){
        if((*nn)[0].size() != 3){
            throw std::runtime_error("The neural Network needs to have 3 inputs");
        }
        if((*nn)[(*nn).size()-1].size() != 3){
            throw std::runtime_error("The neural Network needs to have 3 outputs");
        }
        sf::OutputSoundFile osf;
        osf.openFromFile(soundOutput,44100,2);
        sf::Int16 samplesBuffer[height];
        sf::Uint64 count = height;
        for(auto x=0;x<width;++x){
            auto counter = 0;
            for(auto y=0;y<height;++y){
                auto norm_x = (2*(x/width))-1;
                auto norm_y = (2*(y/height))-1;
                auto d = std::sqrt(((norm_x/2)^2) + ((norm_y /2)^2));
                std::vector<double> inputs;
                inputs.emplace_back((double)x-(width/2));
                inputs.emplace_back((double)y-(height/2));
                inputs.emplace_back((double)d);
                nn->setInputs(std::move(inputs));
                auto audio = nn->run();
                nn->reset();
                if(counter > 2){
                    counter = 0;
                }
                if(counter == 0){
                    samplesBuffer[y] = static_cast<sf::Int16>(10000 * (x*y) * std::cosh(audio[counter] * 440 * (audio[counter+1] * 3.1415f) * audio[counter] / 44100));
                }else if(counter == 1){
                    samplesBuffer[y] = static_cast<sf::Int16>(10000 * (y*d) * std::sin(audio[counter] * 440 * (audio[counter-1] * 3.1415f) * audio[counter] / 44100));
                }else if(counter == 2){
                    samplesBuffer[y] = static_cast<sf::Int16>(10000 * (x*d) * std::cos(audio[counter] * 440 * (audio[counter] * 3.1415f) * audio[counter] / 44100));
                }
                ++counter;
            }
            osf.write(samplesBuffer,count);
        }
    }
    void generateSoundFromColor(const std::string& Imagefilename, NeuralNetwork* nn, const std::string& soundOutput){
        sf::Image imgInput;
        if(!imgInput.loadFromFile(Imagefilename)){
            throw std::runtime_error(Imagefilename + " Couldn't be loaded");
        }
        generateSoundFromColor(imgInput, nn,soundOutput);
    }
    void generateSoundFromColor(sf::Image& imageInput, NeuralNetwork* nn, const std::string& soundOutput){
        if((*nn)[0].size() != 3){
            throw std::runtime_error("The neural Network needs to have 3 inputs");
        }
        if((*nn)[(*nn).size()-1].size() != 3){
            throw std::runtime_error("The neural Network needs to have 3 outputs");
        }
        const int width = imageInput.getSize().x;
        const int height = imageInput.getSize().y;
        sf::OutputSoundFile osf;
        osf.openFromFile(soundOutput,44100,2);
        sf::Int16 samplesBuffer[height];
        sf::Uint64 count = height;
        for(auto x=0;x<width;++x){
            auto counter = 0;
            for(auto y=0;y<height;++y){
                auto norm_x = (2*(x/width))-1;
                auto norm_y = (2*(y/height))-1;
                auto d = std::sqrt(((norm_x/2)^2) + ((norm_y /2)^2));
                std::vector<double> inputs;
                auto imgColor = imageInput.getPixel(x,y);
                inputs.emplace_back((double)imgColor.r);
                inputs.emplace_back((double)imgColor.g);
                inputs.emplace_back((double)imgColor.b);
                nn->setInputs(std::move(inputs));
                auto audio = nn->run();
                nn->reset();
                if(counter > 2){
                    counter = 0;
                }
                if(counter == 0){
                    samplesBuffer[y] = static_cast<sf::Int16>(10000 * (x*y) * std::cosh(audio[counter] * 440 * (audio[counter+1] * 3.1415f) * audio[counter] / 44100));
                }else if(counter == 1){
                    samplesBuffer[y] = static_cast<sf::Int16>(10000 * (y*d) * std::sin(audio[counter] * 440 * (audio[counter-1] * 3.1415f) * audio[counter] / 44100));
                }else if(counter == 2){
                    samplesBuffer[y] = static_cast<sf::Int16>(10000 * (x*d) * std::cos(audio[counter] * 440 * (audio[counter] * 3.1415f) * audio[counter] / 44100));
                }
                ++counter;
            }
            osf.write(samplesBuffer,count);
        }
    }
}