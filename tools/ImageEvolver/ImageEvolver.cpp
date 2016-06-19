#include <SFML/Graphics.hpp>
#include <utility>
#include <memory>
#include <chrono>
#include <thread>

#include <EvoAI/Genome.hpp>
#include <EvoAI/Utils.hpp>


sf::Image createImage(EvoAI::Genome* g, int width = 250, int height = 250, bool bw = false) noexcept;
void updateImages(std::vector<std::unique_ptr<EvoAI::Genome>>& genomes, std::vector<sf::Texture>& textures, std::vector<sf::Sprite>& sprites, std::size_t count,bool bw = false);
int main(int argc, char **argv){
    bool running = true;
    bool loading = false;
    bool bw = false;
    auto size = 17u;
    auto index = 0u;
    std::vector<std::unique_ptr<EvoAI::Genome>> genomes;
    std::vector<sf::Texture> textures(size);
    std::vector<sf::Sprite> sprites(size);
    for(auto i=0u;i<size;++i){
        genomes.push_back(std::make_unique<EvoAI::Genome>(3,3,true,true));
        genomes[i]->mutate();
    }
    updateImages(genomes,textures,sprites,size,bw);
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
                    if(event.key.code == sf::Keyboard::S){
                        genomes[index]->writeToFile("EvolvedG" + std::to_string(index) + ".json");
                        EvoAI::Genome::makePhenotype(*genomes[index])->writeToFile("EvolvedNN" + std::to_string(index) + ".json");
                    }
                    if(event.key.code == sf::Keyboard::B){
                        bw = !bw;
                    }
                    if(event.key.code == sf::Keyboard::Num1){
                        genomes[0]->setFitness(genomes[0]->getFitness() + 1);
                        index = 0;
                    }
                    if(event.key.code == sf::Keyboard::Num2){
                        genomes[1]->setFitness(genomes[1]->getFitness() + 1);
                        index = 1;
                    }
                    if(event.key.code == sf::Keyboard::Num3){
                        genomes[2]->setFitness(genomes[2]->getFitness() + 1);
                        index = 2;
                    }
                    if(event.key.code == sf::Keyboard::Num4){
                        genomes[3]->setFitness(genomes[3]->getFitness() + 1);
                        index = 3;
                    }
                    if(event.key.code == sf::Keyboard::Num5){
                        genomes[4]->setFitness(genomes[4]->getFitness() + 1);
                        index = 4;
                    }
                    if(event.key.code == sf::Keyboard::G){
                        genomes[index] = std::make_unique<EvoAI::Genome>(3,3,true,true);
                        genomes[index]->mutate();
                    }
                    if(event.key.code == sf::Keyboard::Return){
                        std::thread([&](){
                                loading = true;
                                std::sort(std::begin(genomes),std::end(genomes),
                                    [](std::unique_ptr<EvoAI::Genome>& g1,std::unique_ptr<EvoAI::Genome>& g2){
                                        return (g1->getFitness() > g2->getFitness());
                                    });
                                for(auto i=0u;i<size;++i){
                                    auto selectedFather = EvoAI::random(0,genomes.size()-1);
                                    auto selectedMother = EvoAI::random(0,genomes.size()-1);
                                    genomes[i] = EvoAI::Genome::reproduce(*genomes[selectedFather],*genomes[selectedMother]);
                                    genomes[i]->mutate();
                                }
                                App.setActive(true);
                                updateImages(genomes,textures,sprites,size,bw);
                                App.setActive(false);
                                loading = false;
                        }).detach();
                    }
                    if(event.key.code == sf::Keyboard::Space){
                        loading = !loading;
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
            inputs.emplace_back((double)d);
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
void updateImages(std::vector<std::unique_ptr<EvoAI::Genome>>& genomes, std::vector<sf::Texture>& textures, std::vector<sf::Sprite>& sprites, std::size_t count,bool bw){
    auto imageSize = 250;
    auto counter = 0;
    auto verticalPos = 0;
    auto horizontalPos = 0;
    for(auto i=0u;i<count;++i){
        sf::Texture t;
        if(!t.loadFromImage(createImage(genomes[i].get(),imageSize,imageSize,bw))){
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