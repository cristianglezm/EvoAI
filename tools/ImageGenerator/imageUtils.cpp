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
                auto norm_x = (2*(x/width))-1;
                auto norm_y = (2*(y/height))-1;
                auto d = std::sqrt(((norm_x/2)^2) + ((norm_y /2)^2));
                std::vector<double> inputs;
                inputs.emplace_back(normalize<double>(x, 0.0, 1.0, 0, width));
                inputs.emplace_back(normalize<double>(y, 0.0, 1.0, 0, height));
                inputs.emplace_back(static_cast<double>(d));
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
                inputs.emplace_back(normalize<double>(imgColor.r,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(imgColor.g,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(imgColor.b,0.0,1.0,0.0,255.0));
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
                inputs.emplace_back(normalize<double>(imgColor.r,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(imgColor.g,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(imgColor.b,0.0,1.0,0.0,255.0));
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
        if((*nn)[(*nn).size()-1].size() < 1){
            throw std::runtime_error("EvoAI::generateBWImageFromCoords() : The neural Network needs to at least have 1 output");
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
                inputs.emplace_back(normalize<double>(x, 0.0, 1.0, 0, width));
                inputs.emplace_back(normalize<double>(y, 0.0, 1.0, 0, height));
                inputs.emplace_back(static_cast<double>(d));
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
        if((*nn)[(*nn).size()-1].size() < 1){
            throw std::runtime_error("EvoAI::generateImageFromColorAndCoordinates() : The neural Network needs to at least have 1 output");
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
                inputs.emplace_back(normalize<double>(imgColor.r,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(imgColor.g,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(imgColor.b,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(x, 0.0, 1.0, 0, width));
                inputs.emplace_back(normalize<double>(y, 0.0, 1.0, 0, height));
                inputs.emplace_back(static_cast<double>(d));
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
        if((*nn)[(*nn).size()-1].size() < 3){
            throw std::runtime_error("EvoAI::generateImageFromColorAndCoordinates() : The neural Network needs to at least have 3 outputs");
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
                inputs.emplace_back(normalize<double>(imgColor.r,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(imgColor.g,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(imgColor.b,0.0,1.0,0.0,255.0));
                inputs.emplace_back(normalize<double>(x, 0.0, 1.0, 0, width));
                inputs.emplace_back(normalize<double>(y, 0.0, 1.0, 0, height));
                inputs.emplace_back(static_cast<double>(d));
                nn->setInputs(std::move(inputs));
                auto color = nn->run();
                nn->reset();
                imgOutput.setPixel(x,y,sf::Color(color[0] * 255,color[1] * 255,color[2] * 255));
            }
        }
        imgOutput.saveToFile(imageOutput);
    }
}
