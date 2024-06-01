#include <SFML/Graphics.hpp>
#include <utility>
#include <memory>
#include <chrono>
#include <atomic>
#include <functional>
#include <future>
#include <thread>

#include <EvoAI/Genome.hpp>
#include <EvoAI/Population.hpp>
#include <EvoAI/Utils.hpp>


sf::Image createImage(EvoAI::Genome* g, int width = 250, int height = 250, bool bw = false) noexcept;
void updateImages(EvoAI::Population<EvoAI::Genome>& p, std::vector<sf::Texture>& textures, std::vector<sf::Sprite>& sprites, std::size_t count,bool bw = false);

int main(int argc, char** argv){
    EvoAI::randomGen().setSeed(std::random_device{}());
    bool running = true;
    std::atomic<bool> loading = false;
    bool bw = false;
    auto size = 17u;
    for(int i=0u;i<argc;++i){
        auto val = std::string(argv[i]);
        if(val == "--help" || val == "-h"){
            std::cout << "Usage: ImageEvolver -n <size> -b" << std::endl;
            std::cout << "-n <size>\t\tnumber of images that is going to display and evolve." << std::endl;
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
    EvoAI::Population<EvoAI::Genome> p(size, 2.0, 2.0, 1.0, 3,2,3,false,true);
    updateImages(p,textures,sprites,size,bw);
    sf::RenderWindow App(sf::VideoMode(1270, 720), "ImageEvolver");
    auto counter = 2u;
    while(running){
        sf::Event event;
        while(App.pollEvent(event)){
            switch(event.type){
                case sf::Event::Closed:
                        running = false;
                    break;
                case sf::Event::KeyReleased:
                    if(event.key.code == sf::Keyboard::Enter){
                                ++counter;
                                loading.store(true, std::memory_order_release);
                        std::thread([&](){
                                p.reproduce(EvoAI::SelectionAlgorithms::Tournament<EvoAI::Genome>{p.getPopulationMaxSize(), 3}, true);
                                App.setActive(true);
                                updateImages(p,textures,sprites,size,bw);
                                App.setActive(false);
                                loading.store(false, std::memory_order_release);
                        }).detach();
                    }
                    if(event.key.code == sf::Keyboard::B){
                        bw = !bw;
                    }
                    break;
                case sf::Event::KeyPressed:
                    if(event.key.code == sf::Keyboard::Down){
                        ++counter;
                        auto v = App.getView();
                        v.move(0,100);
                        App.setView(v);
                    }
                    if(event.key.code == sf::Keyboard::Up){
                        ++counter;
                        auto v = App.getView();
                        v.move(0,-100);
                        App.setView(v);
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    if(event.mouseButton.button == sf::Mouse::Left){
                        sf::Vector2f pos = App.mapPixelToCoords({event.mouseButton.x,event.mouseButton.y},App.getView());
                        auto index = 0u;
                        for(auto& sp:sprites){
                            if(sp.getGlobalBounds().contains(pos)){
                                auto& genomes = p.getMembers();
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
                                auto& genomes = p.getMembers();
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
        auto load = loading.load(std::memory_order_acquire);
        if(load){
            App.setActive(false);
        }else{
            App.setActive(true);
        }
        if(!load && counter > 0){
            --counter;
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
            auto d = EvoAI::distanceCenter<double>(x,y,width,height);
            auto color = nn.forward({static_cast<double>(x), static_cast<double>(y), d});
            nn.reset();
            if(bw){
                imgOutput.setPixel(x,y,sf::Color(color[0]*128+128,color[0]*128+128,color[0]*128+128));
            }else{
                imgOutput.setPixel(x,y,sf::Color(color[0]*128+128,color[1]*128+128,color[2]*128+128));
            }
        }
    }
    return imgOutput;
}

void updateImages(EvoAI::Population<EvoAI::Genome>& p, std::vector<sf::Texture>& textures, std::vector<sf::Sprite>& sprites, std::size_t count,bool bw){
    auto imageSize = 250;
    auto counter = 0;
    auto verticalPos = 0;
    auto horizontalPos = 0;
    auto& genomes = p.getMembers();
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
