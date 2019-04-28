#include "RawAudio.hpp"

namespace EvoAI{
    void generateSoundFromCoordinates(const int& width, const int& height, NeuralNetwork* nn, const std::string& soundOutput){
        if((*nn)[0].size() != 3){
            throw std::runtime_error("The neural Network needs to have 3 inputs");
        }
        if((*nn)[(*nn).size()-1].size() < 3){
            throw std::runtime_error("The neural Network needs to at least have 3 outputs");
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
                inputs.emplace_back(normalize<double>(x, 0.0, 1.0, 0, width));
                inputs.emplace_back(normalize<double>(y, 0.0, 1.0, 0, height));
                inputs.emplace_back(static_cast<double>(d));
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
        if((*nn)[(*nn).size()-1].size() < 3){
            throw std::runtime_error("The neural Network needs to at least have 3 outputs");
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
                inputs.emplace_back(normalize<double>(imgColor.r,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(imgColor.g,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(imgColor.b,0.0,1.0,0.0,255.0));
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
    void generateSoundFromColorAndCoordinates(const std::string& Imagefilename, NeuralNetwork* nn, const std::string& soundOutput){
        sf::Image imgInput;
        if(!imgInput.loadFromFile(Imagefilename)){
            throw std::runtime_error(Imagefilename + " Couldn't be loaded");
        }
        generateSoundFromColorAndCoordinates(imgInput, nn,soundOutput);
    }
    void generateSoundFromColorAndCoordinates(sf::Image& imageInput, NeuralNetwork* nn, const std::string& soundOutput){
        if((*nn)[0].size() != 6){
            throw std::runtime_error("The neural Network needs to have 6 inputs");
        }
        if((*nn)[(*nn).size()-1].size() < 3){
            throw std::runtime_error("The neural Network needs to at least have 3 outputs");
        }
        const int width = imageInput.getSize().x;
        const int height = imageInput.getSize().y;
        sf::OutputSoundFile osf;
        osf.openFromFile(soundOutput,44100,2);
        sf::Int16 samplesBuffer[height];
        sf::Uint64 count = height;
        for(auto x=0;x<width;++x){
            for(auto y=0;y<height;++y){
                auto norm_x = (2*(x/width))-1;
                auto norm_y = (2*(y/height))-1;
                auto d = std::sqrt(((norm_x/2)^2) + ((norm_y /2)^2));
                std::vector<double> inputs;
                auto imgColor = imageInput.getPixel(x,y);
                inputs.emplace_back(normalize<double>(imgColor.r,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(imgColor.g,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(imgColor.b,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(x, 0.0, 1.0, 0, width));
                inputs.emplace_back(normalize<double>(y, 0.0, 1.0, 0, height));
                inputs.emplace_back(static_cast<double>(d));
                nn->setInputs(std::move(inputs));
                auto audio = nn->run();
                nn->reset();
                const int Amplitude = 30000;
                /// TODO @todo
                samplesBuffer[y] = static_cast<sf::Int16>(Amplitude * std::tanh((audio[0] + audio[1] + audio[2]) * (2 * 3.141516)));
                //samplesBuffer[y] = static_cast<sf::Int16>(Amplitude * sf::Color(audio[0] * 255, audio[1] * 255, audio[2] * 255).toInteger());
            }
            osf.write(samplesBuffer,count);
        }
    }
}