#include "Options.hpp"

namespace EvoAI{
        Options::Options() noexcept
        : coefficients{0.01, 0.01, 1.0}
        , fontFilename("EvoAI/data/fonts/MonaspaceRadon-Regular.otf")
        , saveFilename("")
        , loadFilename("")
        , screen(sf::Vector2f(0.0, 0.0), sf::Vector2f(1280.0, 720.0))
        , maxCellNum(100u)
        , rounds(5u)
        , seed(42u)
        , numSamples(20u)
        , epoch(10u)
        , batchSize(12u)
        , learningRate(0.1)
        , compatibilityThreshold(2.0)
        , speciesMaxAge(20)
        , secondsForNextGen(std::chrono::seconds(10))
        , interspecies(false)
        , renderInfo(true)
        , pause(false)
        , help(false)
        , ignoreTimeCounter(false)
        , renderSensorPoints(false)
        , renderBrainActivity(false)
        , writeAverages(false)
        , writeBestBrain(false)
        , training(false){}
        Options::Options(JsonBox::Object o) noexcept
        : coefficients{}
        , fontFilename(o["fontFilename"].tryGetString("EvoAI/data/fonts/MonaspaceRadon-Regular.otf"))
        , saveFilename(o["saveFilename"].tryGetString(""))
        , loadFilename(o["loadFilename"].tryGetString(""))
        , screen(sf::Vector2f(0.0, 0.0), 
                sf::Vector2f(o["screen"]["width"].tryGetFloat(1280.0),
                            o["screen"]["height"].tryGetFloat(720.0)))
        , maxCellNum(std::stoull(o["maxCellNum"].tryGetString("100")))
        , rounds(std::stoull(o["rounds"].tryGetString("10")))
        , seed(std::stoull(o["seed"].tryGetString("42")))
        , numSamples(std::stoull(o["numSamples"].tryGetString("20")))
        , epoch(std::stoull(o["epoch"].tryGetString("10")))
        , batchSize(std::stoull(o["batchSize"].tryGetString("12")))
        , learningRate(o["learningRate"].tryGetDouble(0.1))
        , compatibilityThreshold(o["compatibilityThreshold"].tryGetDouble(2.0))
        , speciesMaxAge(std::stoull(o["speciesMaxAge"].tryGetString("20")))
        , secondsForNextGen(std::chrono::seconds(o["secondsForNextGen"].tryGetInteger(10)))
        , interspecies(o["interspecies"].tryGetBoolean(false))
        , renderInfo(o["renderInfo"].tryGetBoolean(true))
        , pause(o["pause"].tryGetBoolean(false))
        , help(o["help"].tryGetBoolean(false))
        , ignoreTimeCounter(o["ignoreTimeCounter"].tryGetBoolean(false))
        , renderSensorPoints(o["renderSensorPoints"].tryGetBoolean(false))
        , renderBrainActivity(o["renderBrainActivity"].tryGetBoolean(false))
        , writeAverages(o["writeAverages"].tryGetBoolean(false))
        , writeBestBrain(o["writeBestBrain"].tryGetBoolean(false))
        , training(o["training"].tryGetBoolean(false)){
            auto arr = o["coefficients"].getArray();
            coefficients = std::array<float, 3>{arr[0].tryGetFloat(0.01), 
                                                        arr[1].tryGetFloat(0.01), 
                                                        arr[2].tryGetFloat(1.0)};
        }

        JsonBox::Value Options::toJson() const noexcept{
            JsonBox::Value v;
            v["coefficients"] = JsonBox::Array{coefficients[0], coefficients[1], coefficients[2]};
            v["fontFilename"] = fontFilename;
            v["saveFilename"] = saveFilename;
            v["loadFilename"] = loadFilename;
            v["screen"]["width"] = screen.width;
            v["screen"]["height"] = screen.height;
            v["maxCellNum"] = std::to_string(maxCellNum);
            v["rounds"] = std::to_string(rounds);
            v["seed"] = std::to_string(seed);
            v["numSamples"] = std::to_string(numSamples);
            v["epoch"] = std::to_string(epoch);
            v["batchSize"] = std::to_string(batchSize);
            v["learningRate"] = learningRate;
            v["compatibilityThreshold"] = compatibilityThreshold;
            v["speciesMaxAge"] = std::to_string(speciesMaxAge);
            v["secondsForNextGen"] = std::chrono::duration<double>(secondsForNextGen).count();
            v["interspecies"] = interspecies;
            v["renderInfo"] = renderInfo;
            v["pause"] = pause;
            v["help"] = help;
            v["ignoreTimeCounter"] = ignoreTimeCounter;
            v["renderSensorPoints"] = renderSensorPoints;
            v["renderBrainActivity"] = renderBrainActivity;
            v["WriteAverages"] = writeAverages;
            v["WriteBestBrain"] = writeBestBrain;
            v["training"] = training;
            return v;
        }
        Options parseOptions(int argc, const char** argv){
            Options opts;
            for(auto i=0;i<argc;++i){
                    std::string opt(argv[i]);
                if("-h" == opt || "--help" == opt){
                    opts.help = true;
                }else if("-is" == opt || "--interspecies" == opt){
                    opts.interspecies = true;
                }else if("-f" == opt || "--font" == opt){
                    opts.fontFilename = std::string(argv[i+1]);
                }else if("-mnc" == opt || "--maxNumCell" == opt){
                    opts.maxCellNum = std::stoull(argv[i+1]);
                }else if("-r" == opt || "--rounds" == opt){
                    opts.rounds = std::stoull(argv[i+1]);
                }else if("-seed" == opt){
                    opts.seed = std::stoull(argv[i+1]);
                }else if("-numSamples" == opt){
                    opts.training = true;
                    opts.numSamples = std::stoull(argv[i+1]);
                }else if("-epoch" == opt){
                    opts.training = true;
                    opts.epoch = std::stoull(argv[i+1]);
                }else if("-batchSize" == opt){
                    opts.training = true;
                    opts.batchSize = std::stoull(argv[i+1]);
                }else if("-lr" == opt){
                    opts.training = true;
                    opts.learningRate = std::stod(argv[i+1]);;
                }else if("-train" == opt){
                    opts.training = true;
                }else if("-ct" == opt){
                    opts.compatibilityThreshold = std::stod(argv[i+1]);
                }else if("-sma" == opt || "--speciesMaxAge" == opt){
                    opts.speciesMaxAge = std::stoull(argv[i+1]);
                }else if("-ng" == opt || "--nextGen" == opt){
                    opts.secondsForNextGen = std::chrono::seconds(std::stoi(argv[i+1]));
                }else if("-itc" == opt || "--ignoreTimeCounter" == opt){
                    opts.ignoreTimeCounter = true;
                }else if("-c" == opt || "--coefficients" == opt){
                    opts.coefficients = std::array<float, 3>{std::stof(argv[i+1]), std::stof(argv[i+2]), std::stof(argv[i+3])};
                }else if("-scr" == opt || "--screen" == opt){
                    opts.screen = sf::FloatRect(0.0,0.0, std::stof(argv[i+1]), std::stof(argv[i+2]));
                }else if("-rsp" == opt || "--renderSensorPoints" == opt){
                    opts.renderSensorPoints = true;
                }else if("-rba" == opt || "--renderBrainActivity" == opt){
                    opts.renderBrainActivity = true;
                }else if("-s" == opt || "--save" == opt){
                    opts.saveFilename = std::string(argv[i+1]);
                }else if("-l" == opt || "--load" == opt){
                    opts.loadFilename = std::string(argv[i+1]);
                }else if("-writeBB" == opt || "--writeBestBrain" == opt){
                    opts.writeBestBrain = true;
                }else if("-writeAvgs" == opt || "--writeAverages" == opt){
                    opts.writeAverages = true;
                }
            }
            return opts;
        }
}