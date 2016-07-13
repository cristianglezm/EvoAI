#include <SFML/Graphics.hpp>
#include <utility>
#include <memory>
#include <chrono>
#include <thread>

#include <EvoAI/Genome.hpp>
#include <EvoAI/Population.hpp>
#include <EvoAI/Utils.hpp>


sf::Image createImage(EvoAI::Genome* g, int width = 250, int height = 250, bool bw = false) noexcept;
void updateImages(EvoAI::Population& p, std::vector<sf::Texture>& textures, std::vector<sf::Sprite>& sprites, std::size_t count,bool bw = false);

int main(int argc, char **argv){
    bool running = true;
    bool loading = false;
    bool bw = false;
    auto size = 17u;
    for(int i=0u;i<argc;++i){
        auto val = std::string(argv[i]);
        if(val == "--help" || val == "-h"){
            std::cout << "usage: ImageEvolver -n <size> -b" << std::endl;
            std::cout << "-n <size>\t\tnumber of images is going to display and evolve." << std::endl;
            std::cout << "-b\t\tIf the images will be black and white." << std::endl;
            return EXIT_SUCCESS;
        }else if(val == "-n"){
            size = std::stoull(std::string(argv[i+1]));
        }else if(val == "-b"){
            bw = true;
        }
    }
    std::vector<sf::Texture> textures(size);
    std::vector<sf::Sprite> sprites(size);
    EvoAI::Population p(size,3,2,3,true,true);
    updateImages(p,textures,sprites,size,bw);
    sf::RenderWindow App(sf::VideoMode(1270, 720), "ImageEvolver");
    while (running){
        sf::Event event;
        while(App.pollEvent(event)){
            switch(event.type){
                case sf::Event::Closed:
                        running = false;
                    break;
                case sf::Event::KeyReleased:
                    if(event.key.code == sf::Keyboard::Add){
                        sf::View v = App.getView();
                        v.zoom(0.5);
                        App.setView(v);
                    }
                    if(event.key.code == sf::Keyboard::Subtract){
                        sf::View v = App.getView();
                        v.zoom(1.5);
                        App.setView(v);
                    }
                    if(event.key.code == sf::Keyboard::Return){
                        //std::thread([&](){
                                loading = true;
                                p.reproduce(true,EvoAI::Population::SelectionType::TOURNAMENT);
                                App.setActive(true);
                                updateImages(p,textures,sprites,size,bw);
                                App.setActive(false);
                                loading = false;
                        //}).detach();
                    }
                    if(event.key.code == sf::Keyboard::B){
                        bw = !bw;
                    }
                    break;
                case sf::Event::KeyPressed:
                    if(event.key.code == sf::Keyboard::Down){
                        auto v = App.getView();
                        v.move(0,100);
                        App.setView(v);
                    }
                    if(event.key.code == sf::Keyboard::Up){
                        auto v = App.getView();
                        v.move(0,-100);
                        App.setView(v);
                    }
                    if(event.key.code == sf::Keyboard::Right){
                        auto v = App.getView();
                        v.move(100,0);
                        App.setView(v);
                    }
                    if(event.key.code == sf::Keyboard::Left){
                        auto v = App.getView();
                        v.move(-100,0);
                        App.setView(v);
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    if(event.mouseButton.button == sf::Mouse::Left){
                        sf::Vector2f pos = App.mapPixelToCoords({event.mouseButton.x,event.mouseButton.y},App.getView());
                        auto index = 0u;
                        for(auto& sp:sprites){
                            if(sp.getGlobalBounds().contains(pos)){
                                auto& genomes = p.getGenomes();
                                genomes[index]->addFitness(1.0);
                            }
                            ++index;
                        }
                    }
                    if(event.mouseButton.button == sf::Mouse::Right){
                        sf::Vector2f pos = App.mapPixelToCoords({event.mouseButton.x,event.mouseButton.y},App.getView());
                        auto index = 0u;
                        for(auto& sp:sprites){
                            if(sp.getGlobalBounds().contains(pos)){
                                auto& genomes = p.getGenomes();
                                genomes[index]->writeToFile("ImageEvolver-Genome-" + std::to_string(index) + ".json");
                            }
                            ++index;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        if(loading){
            App.setActive(false);
        }else{
            App.setActive(true);
        }
        if(!loading){
            App.clear(sf::Color::Black);
            for(auto& sp:sprites){
                App.draw(sp);
            }
            App.display();
        }
    }
    App.close();
    return 0;
}

sf::Image createImage(EvoAI::Genome* g, int width, int height, bool bw) noexcept{
    auto nn = EvoAI::Genome::makePhenotype(*g);
    sf::Image imgOutput;
    imgOutput.create(width, height);
    for(auto x=0;x<width;++x){
        for(auto y=0;y<height;++y){
            auto norm_x = (2*(x/width))-1;
            auto norm_y = (2*(y/height))-1;
            auto d = std::sqrt(((norm_x/2)^2) + ((norm_y /2)^2));
            std::vector<double> inputs;
            inputs.emplace_back(EvoAI::normalize<double>(x, -1.0, 1.0, 0, width));
            inputs.emplace_back(EvoAI::normalize<double>(y, -1.0, 1.0, 0, height));
            inputs.emplace_back(static_cast<double>(d));
            nn->setInputs(std::move(inputs));
            auto color = nn->run();
            nn->reset();
            if(bw){
                imgOutput.setPixel(x,y,sf::Color(color[0] * 255,color[0] * 255,color[0] * 255));
            }else{
                imgOutput.setPixel(x,y,sf::Color(color[0] * 255,color[1] * 255,color[2] * 255));
            }
        }
    }
    return imgOutput;
}
void updateImages(EvoAI::Population& p, std::vector<sf::Texture>& textures, std::vector<sf::Sprite>& sprites, std::size_t count,bool bw){
    auto imageSize = 250;
    auto counter = 0;
    auto verticalPos = 0;
    auto horizontalPos = 0;
    auto& genomes = p.getGenomes();
    for(auto i=0u;i<count;++i){
        sf::Texture t;
        genomes[i]->mutate(0.1,0.4,0.2,0.8,0.35,0.33,0.7);
        if(!t.loadFromImage(createImage(genomes[i],imageSize,imageSize,bw))){
            std::cout << "Image " + std::to_string(i) + " Load failed" << std::endl;
        }
        textures[i] = t;
        sf::Sprite s(textures[i]);
        if(counter == 5){
            counter = 0;
            verticalPos += imageSize;
            horizontalPos = 0;
        }else{
            horizontalPos = imageSize * counter;
            ++counter;
        }
        s.setPosition(sf::Vector2f(horizontalPos+5,verticalPos+5));
        sprites[i] = s;
    }
}
