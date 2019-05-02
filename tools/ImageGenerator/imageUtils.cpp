#include "imageUtils.hpp"

namespace EvoAI{
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
        if((*nn)[(*nn).size()-1].size() < 3){
            throw std::runtime_error("EvoAI::generateImageFromCoordinates() : The neural Network needs to at least have 3 outputs");
        }
        int width = imageInput.getSize().x;
        int height = imageInput.getSize().y;
        sf::Image imgOutput;
        imgOutput.create(width, height);
        for(auto x=0;x<width;++x){
            for(auto y=0;y<height;++y){
                auto d = EvoAI::distanceCenter<int>(x,y,width,height);
                std::vector<double> inputs;
                inputs.emplace_back(x);
                inputs.emplace_back(y);
                inputs.emplace_back(d);
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0]*128+128,color[1]*128+128,color[2]*128+128));
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
        if((*nn)[(*nn).size()-1].size() < 3){
            throw std::runtime_error("EvoAI::generateImageFromColor() : The neural Network needs to at least have 3 outputs");
        }
        int width = imageInput.getSize().x;
        int height = imageInput.getSize().y;
        sf::Image imgOutput;
        imgOutput.create(width, height);
        for(auto x=0;x<width;++x){
            for(auto y=0;y<height;++y){
                std::vector<double> inputs;
                auto imgColor = imageInput.getPixel(x,y);
                inputs.emplace_back(imgColor.r);
                inputs.emplace_back(imgColor.g);
                inputs.emplace_back(imgColor.b);
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0]*128+128,color[1]*128+128,color[2]*128+128));
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
        if((*nn)[(*nn).size()-1].size() < 1){
            throw std::runtime_error("EvoAI::generateBWImageFromColor() : The neural Network needs to at least have 1 output");
        }
        int width = imageInput.getSize().x;
        int height = imageInput.getSize().y;
        sf::Image imgOutput;
        imgOutput.create(width, height);
        for(auto x=0;x<width;++x){
            for(auto y=0;y<height;++y){
                std::vector<double> inputs;
                auto imgColor = imageInput.getPixel(x,y);
                inputs.emplace_back(imgColor.r);
                inputs.emplace_back(imgColor.g);
                inputs.emplace_back(imgColor.b);
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0]*128+128,color[0]*128+128,color[0]*128+128));
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
        if((*nn)[(*nn).size()-1].size() < 1){
            throw std::runtime_error("EvoAI::generateBWImageFromCoords() : The neural Network needs to at least have 1 output");
        }
        int width = imageInput.getSize().x;
        int height = imageInput.getSize().y;
        sf::Image imgOutput;
        imgOutput.create(width, height);
        for(auto x=0;x<width;++x){
            for(auto y=0;y<height;++y){
                auto d = EvoAI::distanceCenter<int>(x,y,width,height);
                std::vector<double> inputs;
                inputs.emplace_back(x);
                inputs.emplace_back(y);
                inputs.emplace_back(d);
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0]*128+128,color[0]*128+128,color[0]*128+128));
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
        if((*nn)[(*nn).size()-1].size() < 1){
            throw std::runtime_error("EvoAI::generateImageFromColorAndCoordinates() : The neural Network needs to at least have 1 output");
        }
        int width = imageInput.getSize().x;
        int height = imageInput.getSize().y;
        sf::Image imgOutput;
        imgOutput.create(width, height);
        for(auto x=0;x<width;++x){
            for(auto y=0;y<height;++y){
                auto d = EvoAI::distanceCenter<int>(x,y,width,height);
                std::vector<double> inputs;
                auto imgColor = imageInput.getPixel(x,y);
                inputs.emplace_back(imgColor.r);
                inputs.emplace_back(imgColor.g);
                inputs.emplace_back(imgColor.b);
                inputs.emplace_back(x);
                inputs.emplace_back(y);
                inputs.emplace_back(d);
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0]*128+128,color[0]*128+128,color[0]*128+128));
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
        if((*nn)[(*nn).size()-1].size() < 3){
            throw std::runtime_error("EvoAI::generateImageFromColorAndCoordinates() : The neural Network needs to at least have 3 outputs");
        }
        int width = imageInput.getSize().x;
        int height = imageInput.getSize().y;
        sf::Image imgOutput;
        imgOutput.create(width, height);
        for(auto x=0;x<width;++x){
            for(auto y=0;y<height;++y){
                auto d = EvoAI::distanceCenter<int>(x,y,width,height);
                std::vector<double> inputs;
                auto imgColor = imageInput.getPixel(x,y);
                inputs.emplace_back(imgColor.r);
                inputs.emplace_back(imgColor.g);
                inputs.emplace_back(imgColor.b);
                inputs.emplace_back(x);
                inputs.emplace_back(y);
                inputs.emplace_back(d);
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0]*128+128,color[1]*128+128,color[2]*128+128));
            }
        }
        imgOutput.saveToFile(imageOutput);
    }
}
