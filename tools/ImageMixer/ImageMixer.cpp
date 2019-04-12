#include <SFML/Graphics.hpp>
#include <utility>
#include <memory>
#include <chrono>
#include <thread>

#include <EvoAI/Genome.hpp>
#include <EvoAI/Population.hpp>
#include <EvoAI/Utils.hpp>

sf::Image createImageFromImages(EvoAI::Genome* g, EvoAI::NeuralNetwork* nn, std::vector<sf::Image>& imgs, bool bw) noexcept;
void usage();

int main(int argc, char **argv){
    bool optGenome = false;
    bool optGenomeType = false;
    bool optMutate = false;
    bool optReproduce = false;
    std::string genomeType = "0";
    int numHidden = 0;
    std::string genomeFile1 = "genome1.json";
    std::string genomeFile2 = "genome2.json";
    bool optBW = false;
    bool optSaveNN = false;
    std::string saveFileNN = "nn.json";
    bool optSaveGenome = false;
    std::string saveFileGenome = "g.json";
    std::string fileOutput = "image.png";
    std::vector<sf::Image> imagesInputs;
    for(auto i=0;i<argc;++i){
        auto val = std::string(argv[i]);
        if(val == "-g" || val == "--genome"){
            optGenome = true;
            if(std::string(argv[i+1]) == "m"){
                optMutate = true;
                genomeFile1 = std::string(argv[i+2]);
            }else if(std::string(argv[i+1]) == "r"){
                optReproduce = true;
                genomeFile1 = std::string(argv[i+2]);
                genomeFile2 = std::string(argv[i+3]);
            }else{
                genomeFile1 = std::string(argv[i+1]);
            }
        }
        if(val == "-G" || val == "--genome-type"){
            optGenomeType = true;
            if(std::string(argv[i+1]) == "0"){
                genomeType = "0";
                numHidden = 0;
            }else if(std::string(argv[i+1]) == "1"){
                genomeType = "1";
                numHidden = std::stoi(std::string(argv[i+2]));
            }
        }
        if(val == "-bw"){
            optBW = true;
        }
        if(val == "-f" || val == "--file-output"){
            fileOutput = std::string(argv[i+1]);
        }
        if(val == "-sg" || val == "--save-genome"){
            optSaveGenome = true;
            saveFileGenome = std::string(argv[i+1]);
        }
        if(val == "-s" || val == "--save"){
            optSaveNN = true;
            saveFileNN = std::string(argv[i+1]);
        }
        if(val =="--image"){
            auto numImg = std::stoi(argv[i+1]);
            for(auto j=0;j<numImg;++j){
                imagesInputs.emplace_back(sf::Image());
                imagesInputs[j].loadFromFile(std::string(argv[i+(j+2)]));
            }
        }
        if(val == "--help" || val == "-h"){
            std::cout << std::string(argv[0]) << " <options>\n";
            usage();
            return EXIT_FAILURE;
        }
    }
    if(argc < 2){
        usage();
        return EXIT_FAILURE;
    }
    std::unique_ptr<EvoAI::Genome> g = nullptr;
    std::unique_ptr<EvoAI::NeuralNetwork> nn = nullptr;
    if(optGenome){
        if(optMutate){
            std::cout << "Mutating genome " << genomeFile1 << std::endl;
            g = std::make_unique<EvoAI::Genome>(genomeFile1);
            g->mutate();
        }else if(optReproduce){
            auto g1 = std::make_unique<EvoAI::Genome>(genomeFile1);
            auto g2 = std::make_unique<EvoAI::Genome>(genomeFile2);
            std::cout << "Reproducing genomes " << genomeFile1 << " And " << genomeFile2 << std::endl;
            g = EvoAI::Genome::reproduce(*g1,*g2);
            g->setCppn(true);
        }else{
            std::cout << "Loading genome " << genomeFile1 << std::endl;
            g = std::make_unique<EvoAI::Genome>(genomeFile1);
        }
        nn = EvoAI::Genome::makePhenotype(*g);
    }else if(optGenomeType){
        if(genomeType == "0"){
            if(optBW){
                g = std::make_unique<EvoAI::Genome>(3 * imagesInputs.size(),1,true,true);
            }else{
                g = std::make_unique<EvoAI::Genome>(3 * imagesInputs.size(),3,true,true);
            }
        }else if(genomeType == "1"){
            if(optBW){
                g = std::make_unique<EvoAI::Genome>(3 * 20 * imagesInputs.size(),numHidden,1,true,true);
            }else{
                g = std::make_unique<EvoAI::Genome>(3 * 20 * imagesInputs.size(),numHidden,3,true,true);
            }
        }
        nn = EvoAI::Genome::makePhenotype(*g);
    }
    if(optSaveNN){
        std::cout << "Saving Neural Network to " << saveFileNN << " ..." << std::endl;
        nn->writeToFile(saveFileNN);
    }
    if(optSaveGenome){
        std::cout << "Saving Genome to " << saveFileGenome << "..." << std::endl;
        g->writeToFile(saveFileGenome);
    }
    std::cout << "Creating Image..." << std::endl;
    auto imgOutput = createImageFromImages(g.get(),nn.get(),imagesInputs,optBW);
    std::cout << "saving Image to " << fileOutput << std::endl;
    imgOutput.saveToFile(fileOutput);
}

sf::Image createImageFromImages(EvoAI::Genome* g, EvoAI::NeuralNetwork* nn, std::vector<sf::Image>& imgs, bool bw) noexcept{
    sf::Image imgOutput;
    if(imgs.empty()){
        return imgOutput;
    }
    auto width = imgs[0].getSize().x;
    auto height = imgs[0].getSize().y;
    imgOutput.create(width, height);
    for(auto x=0u;x<width;++x){
        for(auto y=0u;y<height;++y){
            std::vector<double> inputs;
            for(auto i=0u;i<imgs.size();++i){
                if(x <= imgs[i].getSize().x && y <= imgs[i].getSize().y){
                    auto imgColor = imgs[i].getPixel(x,y);
                    inputs.emplace_back(EvoAI::normalize<double>(imgColor.r,-1.0,1.0,0.0,255.0));
                    inputs.emplace_back(EvoAI::normalize<double>(imgColor.g,-1.0,1.0,0.0,255.0));
                    inputs.emplace_back(EvoAI::normalize<double>(imgColor.b,-1.0,1.0,0.0,255.0));
                }
            }
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
void usage(){
    std::cout << "-g, --genome [m|r] <filename> [<filename> with r]\tload a genome json file\n";
    std::cout << "\t\t\t\t\tWith m will mutate the genome.\n\t\t\t\t\tWith r will combine two genomes, without m or r will load the genome.\n";
    std::cout << "-G, --genome-type <type> <numHidden>\twill generate a genome of the type specified\n\t\t\t\t\t\ttypes:\n\t\t\t\t\t\t\t" <<
                                                            "0. Without hidden neurons\n\t\t\t\t\t\t\t1. With hidden neurons.\n";
    std::cout << "-bw\t\t\t\t\tthe output image is black and white.\n";
    std::cout << "-s, --save <filename>\t\t\twill save the neural network generated.\n";
    std::cout << "-sg, --save-genome <filename>\t\twill save the genome generated.\n";
    std::cout << "-f, --file-output <filename>\t\timage that will output.\n";
    std::cout << "--image <n> <filename>...\t\t\tload n images and mix them.\n";
    std::cout << "-h, --help\t\t\t\thelp menu (This)\n";
}