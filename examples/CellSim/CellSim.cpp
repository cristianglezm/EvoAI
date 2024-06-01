#include "CellSim.hpp"

namespace EvoAI{
    CellSim::CellSim(const Options& opt)
    : opts{opt}
    , bounds(opt.screen.left, opt.screen.top, opt.screen.width, opt.screen.height)
    , win(sf::VideoMode(opt.screen.width, opt.screen.height), "CellSim", sf::Style::Default)
    , cells()
    , FPSFont()
    , FPSText()
    , sizeText()
    , genInfo()
    , pop(nullptr)
    , avgs(nullptr)
    , FPSNumFrames(0)
    , gen(0)
    , lastAlive(0)
    , FPSUpdateTime(sf::Time::Zero)
    , nextGenTimer(opts.secondsForNextGen)
    , trainer(opts.numSamples, opts.batchSize, opts.epoch, opts.learningRate){
        cells.reserve(opts.maxCellNum);
        pop = std::make_unique<Population<Cell*>>([this](){
                    return this->createCells();
            }, opts.maxCellNum, opts.coefficients[0], opts.coefficients[1], opts.coefficients[2]);
        avgs = std::make_unique<Averages>();
        pop->setCompatibilityThreshold(opts.compatibilityThreshold);
        pop->setMaxAge(opts.speciesMaxAge);
        setupGUI();
    }
    CellSim::CellSim(JsonBox::Object o)
    : opts{o["options"].getObject()}
    , bounds(opts.screen.left, opts.screen.top, opts.screen.width, opts.screen.height)
    , win(sf::VideoMode(opts.screen.width, opts.screen.height), "CellSim", sf::Style::Default)
    , cells()
    , FPSFont()
    , FPSText()
    , sizeText()
    , genInfo()
    , pop(nullptr)
    , avgs(nullptr)
    , FPSNumFrames(0)
    , gen(std::stoull(o["gen"].tryGetString("0")))
    , lastAlive(0)
    , FPSUpdateTime(sf::Time::Zero)
    , nextGenTimer(opts.secondsForNextGen)
    , trainer(opts.numSamples, opts.batchSize, opts.epoch, opts.learningRate){
        avgs = std::make_unique<Averages>(o["avgs"].getObject());
        cells.reserve(opts.maxCellNum);
        pop = std::make_unique<Population<Cell*>>();
        for(auto& v:o["cells"].getArray()){
            cells.emplace_back(v.getObject());
            // we need to reset the ID
            cells.back().setID(genID());
            // and setPosition as we don't save those only save Genome and EvoVector.
            cells.back().setPosition(randomGen().random(0.f, bounds.width), 
                                        randomGen().random(0.f, bounds.height));
            pop->addMember(&cells.back(), opts.coefficients[0], opts.coefficients[1], opts.coefficients[2]);
        }
        lastAlive = cells.size() - 1;
        pop->setPopulationMaxSize(opts.maxCellNum);
        pop->setCompatibilityThreshold(opts.compatibilityThreshold);
        pop->setMaxAge(opts.speciesMaxAge);
        setupGUI();
    }
    JsonBox::Value CellSim::toJson() const noexcept{
        JsonBox::Value v;
        v["options"] = opts.toJson();
        JsonBox::Array arr;
        arr.reserve(opts.maxCellNum);
        for(auto& c:cells){
            arr.emplace_back(c.toJson());
        }
        v["cells"] = arr;
        v["avgs"] = avgs->toJson();
        v["gen"] = std::to_string(gen);
        return v;
    }
    void CellSim::writeToFile(std::string_view filename) const noexcept{
        JsonBox::Value v;
        v["Version"] = 1.0;
        v["CellSim"] = toJson();
#ifdef NDEBUG
        v.writeToFile(filename.data(), false, false);
#else
        v.writeToFile(filename.data(), true, false);
#endif
    }
    void CellSim::run() noexcept{
        win.setFramerateLimit(60);
        sf::Clock clock;
        sf::Time timeSinceLastUpdate = sf::Time::Zero;
        while(win.isOpen()){
            sf::Time dt = clock.restart();
            timeSinceLastUpdate += dt;
            while(timeSinceLastUpdate > TimePerFrame){
                timeSinceLastUpdate -= TimePerFrame;
                sf::Event event;
                while(win.pollEvent(event)){
                    handleInput(event);
                }
                update(TimePerFrame);
            }
            updateFPS(dt);
            render();
        }
        if(opts.writeAverages){
            // use python ../tools/showMultiPlot.py CellSim-avgs.txt
            avgs->writeToFile("CellSim-avgs.txt");
        }
        if(!opts.saveFilename.empty()){
            writeToFile(opts.saveFilename);
        }
        if(opts.writeBestBrain){
            auto best = pop->getBestMember();
            if(best){
                best->getBrain().writeDotFile("CellSim-bestBrain.dot");
                best->getBrain().writeToFile("CellSim-bestBrain.json");
            }
        }
    }
    void CellSim::usage() noexcept{
        std::cout << "CellSim [options]\n";
        std::cout << "CellSim -is -mnc 100 -r 10 -ng 30 -c 0.01 0.01 1.0 -scr 1280 720 -seed 4242\n";
        std::cout << "CellSim -mnc 250 -r 5 -c 0.01 0.1 1.0 -scr 1920 1080 -seed 3253 -itc\n";
        std::cout << "\t-is, --interspecies\t\tif interspecies reproduction is allowed.\n";
        std::cout << "\t-f, --font <TFF file>\t\tTFF file to load.\n";
        std::cout << "\t-mnc, --maxNumCell <n>\t\tmax number of cells.\n";
        std::cout << "\t-r, --rounds <n>\t\tRounds for the Selection Algorithm (Tournament),\n";
        std::cout << "\t-seed <n>\t\t\tseed for randomness.(default 42).\n";
        std::cout << "\t-numSamples <n>\t\t\tnumber of samples.(default 20).\n";
        std::cout << "\t-epoch <n>\t\t\tepoch for training.(default 10).\n";
        std::cout << "\t-batchSize <n>\t\t\tBatch size.(default 12).\n";
        std::cout << "\t-lr <float>\t\t\tlearning rate (default 0.1)\n";
        std::cout << "\t-train\t\t\t\tpretrain the cells with default values\n";
        std::cout << "\t-ct <float>\t\t\tcompatibility Threshold for speciation.\n";
        std::cout << "\t-sma, --speciesMaxAge <n>\t\tmax Age for species\n";
        std::cout << "\t-ng, --nextGen <seconds>\t\tseconds to the next generation,\n";
        std::cout << "\t-itc, --ignoreTimeCounter\t\tignore the time counter for next generation.\n";
        std::cout << "\t-c, --coefficients <float float float>\timportance of Genome, EvoString.\n";
        std::cout << "\t-scr, --screen <width height>\t\tscreen resolution.\n";
        std::cout << "\t-rsp, --renderSensorPoints\t\trender points of contact\n";
        std::cout << "\t-rba, --renderBrainActivity\t\trender brain Activity\n";
        std::cout << "\t-s, --save <filename>\t\t\tfilename to save the simulation.\n";
        std::cout << "\t-l, --load <filename>\t\t\tfilename to load the simulation.\n";
        std::cout << "\t-writeBB, --writeBestBrain\t\twrite the best cell::brain\n";
        std::cout << "\t-writeAvgs, --writeAverages\t\twrite the averages of the Stats from Cells\n";
        std::cout << "\t-h, --help\t\t\t\tThis menu." << std::endl;
    }
    void CellSim::handleInput(sf::Event& e) noexcept{
        switch(e.type){
            case sf::Event::Closed:
                    win.close();
                break;
            case sf::Event::KeyReleased:
                if(e.key.code == sf::Keyboard::I){
                    opts.renderInfo = !opts.renderInfo;
                }
                if(e.key.code == sf::Keyboard::P){
                    opts.pause = !opts.pause;
                }
                if(e.key.code == sf::Keyboard::S){
                    opts.interspecies = !opts.interspecies;
                }
                if(e.key.code == sf::Keyboard::B){
                    opts.renderBrainActivity = !opts.renderBrainActivity;
                }
                if(e.key.code == sf::Keyboard::R){
                    opts.renderSensorPoints = !opts.renderSensorPoints;
                }
                if(e.key.code == sf::Keyboard::T){
                    opts.training = !opts.training;
                }
                if(e.key.code == sf::Keyboard::Escape){
                    win.close();
                }
                break;
            case sf::Event::MouseButtonReleased:
                if(e.mouseButton.button == sf::Mouse::Button::Right){
                    nextGeneration();
                }
                if(e.mouseButton.button == sf::Mouse::Button::Left){
                    pop->clear();
                    cells.clear();
                    lastAlive = 0;
                    gen = 0;
                    pop->regrowPopulation([this](){
                        return this->createCells();
                    }, opts.coefficients[0], opts.coefficients[1], opts.coefficients[2]);
                    genInfo.setString("Generation: " + std::to_string(gen) + " - AVG Fitness: " + 
                                                        std::to_string(pop->computeAvgFitness()) + " - Species: " + 
                                                        std::to_string(pop->getSpeciesSize()));
                }
                break;
            default: // remove warning of unused cases.
                break;
        }
    }
    Cell* CellSim::createCells() noexcept{
        // handle initial creation of population and regrowPopulation
        if(cells.size() < opts.maxCellNum){
            auto& c = cells.emplace_back(makeCellGenome());
            c.setID(genID());
            c.setPosition(randomGen().random(0.f, bounds.width), 
                            randomGen().random(0.f, bounds.height));
            lastAlive = cells.size() - 1;
            return &c;
        }else{
            if(lastAlive < (opts.maxCellNum - 1)){
                ++lastAlive;
            }
            Cell* c = &cells[lastAlive];
            auto pos = c->getPosition();
            *c = makeCellGenome();
            c->setPosition(pos);
            c->setID(genID());
            return c;
        }
    }
    void CellSim::replace(std::vector<Cell*>& toReplace, std::vector<Cell>& toAdd) noexcept{
        if(toReplace.empty()){
            for(auto& add:toAdd){
                if(std::distance(std::begin(cells) + lastAlive, std::end(cells)) > 0){
                    Cell* c = &cells[++lastAlive];
                    auto pos = c->getPosition();
                    pop->removeMember(*c);
                    *c = std::move(add);
                    c->setPosition(pos);
                    c->setID(genID());
                    pop->addMember(c, opts.coefficients[0], opts.coefficients[1], opts.coefficients[2]);
                }else{
                    auto& c = cells.emplace_back(std::move(add));
                    c.setID(genID());
                    c.setPosition(randomGen().random(0.f, bounds.width), 
                                    randomGen().random(0.f, bounds.height));
                    pop->addMember(&c, opts.coefficients[0], opts.coefficients[1], opts.coefficients[2]);
                    if(lastAlive < (opts.maxCellNum - 1)){
                        ++lastAlive;
                    }
                }
            }
        }else if(toReplace.size() == toAdd.size()){
            for(auto i=0u;i<toAdd.size();++i){
                auto pos = toReplace[i]->getPosition();
                pop->removeMember(*toReplace[i]);
                toAdd[i].setID(genID());
                toAdd[i].setPosition(pos);
                *toReplace[i] = std::move(toAdd[i]);
                pop->addMember(toReplace[i], opts.coefficients[0], opts.coefficients[1], opts.coefficients[2]);
                if(lastAlive < (opts.maxCellNum - 1)){
                    ++lastAlive;
                }
            }
        }
    }
    void CellSim::removeCellsFromSpecies(std::vector<std::size_t>&& ids) noexcept{
        for(auto& id:ids){
            for(auto i=0u;i<(lastAlive + 1);++i){
                if(cells[i].getSpeciesID() == id){
                    std::swap(cells[i], cells[lastAlive]);
                    if(lastAlive > 0){
                        --lastAlive;
                    }
                }
            }
        }
    }
    void CellSim::nextGeneration() noexcept{
        avgs->calcAvgs(*pop);
        // we use an specialization of Tournament<Cell*> to only select those cells that are not alive. ("Tournament.hpp")
        auto sa = SelectionAlgorithms::Tournament<Cell*>{opts.maxCellNum, opts.rounds};
        auto res = pop->reproduce(sa, opts.interspecies);
        removeCellsFromSpecies(pop->increaseAgeAndRemoveOldSpecies());
        replace(res.first, res.second);
        // remove dead cells to let the pop regrow in case of extinction
        for(auto i=(lastAlive+1);i<cells.size();++i){
            pop->removeMember(cells[i]);
        }
        pop->regrowPopulation([this](){ 
                    return this->createCells();
                }, opts.coefficients[0], opts.coefficients[1], opts.coefficients[2]);
        ++gen;
        genInfo.setString("Generation: " + std::to_string(gen) + " - AVG Fitness: " + 
                                            std::to_string(pop->computeAvgFitness()) + " - Species: " + 
                                            std::to_string(pop->getSpeciesSize()));
    }
    void CellSim::setupGUI() noexcept{
        FPSFont.loadFromFile(opts.fontFilename);
        FPSText.setFont(FPSFont);
        FPSText.setPosition(bounds.width - 75,bounds.height - 45);
        FPSText.setCharacterSize(25u);
        sizeText.setFont(FPSFont);
        sizeText.setPosition(bounds.width - 175, 10);
        sizeText.setCharacterSize(25u);
        sizeText.setString("cells: " + std::to_string(cells.size()));
        genInfo.setFont(FPSFont);
        genInfo.setPosition(10, bounds.height - 45);
        genInfo.setCharacterSize(25u);
        genInfo.setString("Generation: " + std::to_string(gen) + " - AVG Fitness: " + 
                                            std::to_string(pop->computeAvgFitness()) + " - Species: " + 
                                            std::to_string(pop->getSpeciesSize()));
    }
    sf::VertexArray CellSim::getBrainActivity(Cell& cell) noexcept{
        auto& brain = cell.getBrain();
        float left = 10.0;
        float top = 10.0;
        float widthPad = 2.0;
        float heightPad = 2.0;
        switch(cell.getBodyType()){
            case 2:
                    left = 7.0;
                    top = 8.0;
                break;
            case 3:
                    left = 7.0;
                    top = 2.0;
                break;
        }
        sf::VertexArray va(sf::PrimitiveType::Quads);
        auto comparator = [](auto& n1, auto& n2){
            return n1.getOutput() < n2.getOutput();
        };
        auto initialPos = cell.getPosition() + sf::Vector2f(left, top);
        for(auto i=0u;i<brain.size();++i){
            auto pos = initialPos + sf::Vector2f(widthPad * i, 0);
            for(auto j=0u;j<brain[i].size();++j){
                sf::Color isActive = sf::Color::Red;
                if(i == (brain.size() - 1)){
                    auto& neurons = brain[i].getNeurons();
                    auto index = Argmax(std::begin(neurons), std::end(neurons), comparator);
                    isActive = (index == j) ? sf::Color::Green : sf::Color::Red;
                }else{
                    isActive = brain[i][j].getOutput() > 0.0 ? sf::Color::Green : sf::Color::Red;
                }
                // left
                va.append(sf::Vertex(pos, isActive));
                // width
                va.append(sf::Vertex(pos + sf::Vector2f(widthPad, 0), isActive));
                // height
                va.append(sf::Vertex(pos + sf::Vector2f(widthPad, heightPad), isActive));
                // top
                va.append(sf::Vertex(pos + sf::Vector2f(0, heightPad), isActive));
                pos += sf::Vector2f(0, heightPad + 1.0);
            }
        }
        return va;
    }
    Genome CellSim::makeCellGenome() noexcept{
        constexpr auto inputSize = 13u;
        constexpr auto hiddenSize = 8u;
        constexpr auto outputSize = 12u;
        if(randomGen().random(0.5)){
            if(opts.training){
                return trainer.train(Genome(inputSize, outputSize, true, false));
            }
            return Genome(inputSize, outputSize, true, false);
        }
        Genome g;
        // make inputs nodes
        for(auto i=0u;i<inputSize;++i){
            g.addGene(NodeGene(0, i, Neuron::Type::INPUT, Neuron::ActivationType::IDENTITY));
        }
        // [inputs] -> [hidden] -> [outputs]
        // [9] stamina to idle [0]
        g.addGene(NodeGene(1, 0, Neuron::Type::HIDDEN, Neuron::ActivationType::COSINE));
        // group [0-7] sensors connect to 2 hidden neurons[1, 1][1, 2] that connect to moves[0-8]
        for(auto i=1u;i<3u;++i){
            g.addGene(NodeGene(1, i, Neuron::Type::HIDDEN, Neuron::ActivationType::RELU));
        }
        // group [8] health, [9] stamina and [10] hunger to attacking [9] and moves(0-8)
        g.addGene(NodeGene(1, 3, Neuron::Type::HIDDEN, Neuron::ActivationType::COSINE));
        // group [8] health and [10] hunger to eating [10] and moves(0-8)
        g.addGene(NodeGene(1, 4, Neuron::Type::HIDDEN, Neuron::ActivationType::COSINE));
        // [11] targetAtk to [1,5] to defending[11] and moves [0-8]
        g.addGene(NodeGene(1, 5, Neuron::Type::HIDDEN, Neuron::ActivationType::RELU));
        // [12] targetAlive to [1, 6] to eating [10]
        g.addGene(NodeGene(1, 6, Neuron::Type::HIDDEN, Neuron::ActivationType::COSINE));
        // [12] targetAlive [1, 7] to atk [9]
        g.addGene(NodeGene(1, 7, Neuron::Type::HIDDEN, Neuron::ActivationType::RELU));
        // make outputs nodes
        for(auto i=0u;i<outputSize;++i){
            g.addGene(NodeGene(2, i, Neuron::Type::OUTPUT, Neuron::ActivationType::SIGMOID));
        }
        // make connections
        // [9] stamina to idle [0]
        g.addGene(ConnectionGene(Link(0, 9), Link(1, 0), randomGen().random(-1.0, 1.0, hiddenSize)));
        g.addGene(ConnectionGene(Link(1, 0), Link(2, 0), randomGen().random(-1.0, 1.0, hiddenSize)));
        g.addGene(ConnectionGene(Link(2, 0), Link(1, 0), 1.0));
        // group [0-7] sensors to [1, 1] and [1, 2] to moves[0-8] and attacking[9] and eating [10]
        for(auto i=1u;i<3;++i){
            for(auto j=0;j<8;++j){
                g.addGene(ConnectionGene(Link(0, j), Link(1, i), randomGen().random(-1.0, 1.0, inputSize + outputSize)));
            }
            g.addGene(ConnectionGene(Link(1, i), Link(2, 9), randomGen().random(-1.0, 1.0, inputSize + outputSize)));
            g.addGene(ConnectionGene(Link(1, i), Link(2, 10), randomGen().random(-1.0, 1.0, inputSize + outputSize)));
            for(auto j=0u;j<9;++j){
                g.addGene(ConnectionGene(Link(1, i), Link(2, j), randomGen().random(-1.0, 1.0, hiddenSize)));
            }
        }
        // [8] health, [9] stamina, [10] hunger to [1, 3] to attacking [9] and moves(0-8)
        g.addGene(ConnectionGene(Link(0, 8), Link(1, 3), randomGen().random(-1.0, 1.0, inputSize + outputSize)));
        g.addGene(ConnectionGene(Link(0, 9), Link(1, 3), randomGen().random(-1.0, 1.0, inputSize + outputSize)));
        g.addGene(ConnectionGene(Link(0, 10), Link(1, 3), randomGen().random(-1.0, 1.0, inputSize + outputSize)));
        for(auto i=0u;i<9;++i){
            g.addGene(ConnectionGene(Link(1, 3), Link(2, i), randomGen().random(-1.0, 1.0, hiddenSize)));
        }
        g.addGene(ConnectionGene(Link(1, 3), Link(2, 9), randomGen().random(-1.0, 1.0, hiddenSize)));
        // group [8] health and [10] hunger to [1,4] to eating [10] and moves(0-8)
        g.addGene(ConnectionGene(Link(0, 8), Link(1, 4), randomGen().random(-1.0, 1.0, inputSize + outputSize)));
        g.addGene(ConnectionGene(Link(0, 10), Link(1, 4), randomGen().random(-1.0, 1.0, inputSize + outputSize)));
        for(auto i=0u;i<9;++i){
            g.addGene(ConnectionGene(Link(1, 4), Link(2, i), randomGen().random(-1.0, 1.0, hiddenSize)));
        }
        g.addGene(ConnectionGene(Link(1, 4), Link(2, 10), randomGen().random(-1.0, 1.0, hiddenSize)));
        // [11] targetAtk to [1,5] to defending[11] and moves [0-8]
        g.addGene(ConnectionGene(Link(0, 11), Link(1, 5), randomGen().random(-1.0, 1.0, inputSize + outputSize)));
        g.addGene(ConnectionGene(Link(1, 5), Link(2, 11), randomGen().random(-1.0, 1.0, hiddenSize)));
        for(auto i=0u;i<9;++i){
            g.addGene(ConnectionGene(Link(1, 5), Link(2, i), randomGen().random(-1.0, 1.0, hiddenSize)));
        }
        // [12] targetAlive to [1, 6] to eating [10]
        g.addGene(ConnectionGene(Link(0, 12), Link(1, 6), randomGen().random(-1.0, 1.0, inputSize + outputSize)));
        g.addGene(ConnectionGene(Link(1, 6), Link(2, 10), randomGen().random(-1.0, 1.0, hiddenSize)));
        // [12] targetAlive [1, 7] to atk [9]
        g.addGene(ConnectionGene(Link(0, 12), Link(1, 7), randomGen().random(-1.0, 1.0, inputSize + outputSize)));
        g.addGene(ConnectionGene(Link(1, 7), Link(2, 9), randomGen().random(-1.0, 1.0, hiddenSize)));
        g.setRecurrentAllowed(true);
        // we dont want to change activation functions.
        g.setCppn(false);
        if(opts.training){
            return trainer.train(std::move(g));
        }
        return g;
    }
    void CellSim::checkCollision(Cell& c1, Cell& c2) noexcept{
        if(&c1 == &c2){
            return;
        }
        if(c1.canBeEaten() && c2.canBeEaten()){
            return;
        }
        if(c1.getBounds().intersects(c2.getBounds())){
                c1.setTarget(&c2);
                auto c1bounds = c1.getBounds();
                auto c2bounds = c2.getBounds();
                // right collision
                if(c1bounds.left < c2bounds.left &&
                    c1bounds.left + c1bounds.width < c2bounds.left + c2bounds.width &&
                    c1bounds.top < c2bounds.top + c2bounds.height &&
                    c1bounds.top + c1bounds.height > c2bounds.top){
                        c1.setPosition(c2bounds.left - c1bounds.width, c1bounds.top);
                }
                // left collision
                if(c1bounds.left > c2bounds.left &&
                    c1bounds.left + c1bounds.width > c2bounds.left + c2bounds.width &&
                    c1bounds.top < c2bounds.top + c2bounds.height &&
                    c1bounds.top + c1bounds.height > c2bounds.top){
                        c1.setPosition(c2bounds.left + c2bounds.width, c1bounds.top);
                }
                // bottom collision
                if(c1bounds.top < c2bounds.top &&
                    c1bounds.top + c1bounds.height < c2bounds.top + c2bounds.height &&
                    c1bounds.left < c2bounds.left + c2bounds.width &&
                    c1bounds.left + c1bounds.width > c2bounds.left){
                        c1.setPosition(c1bounds.left, c2bounds.top - c1bounds.height);
                }
                // top collision
                if(c1bounds.top > c2bounds.top &&
                    c1bounds.top + c1bounds.height > c2bounds.top + c2bounds.height &&
                    c1bounds.left < c2bounds.left + c2bounds.width &&
                    c1bounds.left + c1bounds.width > c2bounds.left){
                        c1.setPosition(c1bounds.left, c2bounds.top + c2bounds.height);
                }
        }
    }
    void CellSim::checkBounds(Cell& c) noexcept{
        auto pos = c.getPosition();
        auto cBounds = c.getBounds();
        sf::Vector2f p = pos;
        if(pos.x >= opts.screen.width) p.x = opts.screen.left + cBounds.width;
        if(pos.x <= opts.screen.left) p.x = opts.screen.width - cBounds.width;
        if(pos.y >= opts.screen.height) p.y = opts.screen.top + cBounds.height;
        if(pos.y <= opts.screen.top) p.y = opts.screen.height - cBounds.height;
        if(!opts.screen.contains(pos)){
            c.setPosition(p);
        }
    }
    void CellSim::update(sf::Time dt) noexcept{
        if(opts.pause){
            return;
        }
        sizeText.setString("cells: " + std::to_string(lastAlive + 1));
        for(auto i=0u;i<(lastAlive + 1);++i){
            checkBounds(cells[i]);
            for(auto j=0u;j<(lastAlive + 1);++j){
                checkCollision(cells[i], cells[j]);
            }
            cells[i].update(dt);
            if(!cells[i].isAlive() && !cells[i].canBeEaten()){
                // we need to remove the dead cell and the lastAlive cell from pop
                // before the swap otherwise pop->removeMember wont work.
                // species would kept the wrong pointer as its data is swapped 
                // and after some loops the cells would not be able to be removed as its speciesID would be wrong.
                pop->removeMember(cells[i]);
                pop->removeMember(cells[lastAlive]);
                std::swap(cells[i], cells[lastAlive]);
                // we add the cells to correct the pointers in pop
                pop->addMember(&cells[i], opts.coefficients[0], opts.coefficients[1], opts.coefficients[2]);
                pop->addMember(&cells[lastAlive], opts.coefficients[0], opts.coefficients[1], opts.coefficients[2]);
                if(lastAlive > 0){
                    --lastAlive;
                }
            }
        }
        if(!opts.ignoreTimeCounter){
            if(nextGenTimer.update()){
                nextGeneration();
            }
        }
    }
    void CellSim::updateFPS(sf::Time dt) noexcept{
        FPSUpdateTime += dt;
        FPSNumFrames += 1;
        if(FPSUpdateTime >= sf::seconds(1.0f)){
            if(FPSNumFrames < 10){
                FPSText.setFillColor(sf::Color::Red);
            }else if(FPSNumFrames > 59){
                FPSText.setFillColor(sf::Color::Green);
            }else if(FPSNumFrames < 30){
                FPSText.setFillColor(sf::Color::Yellow);
            }
            FPSText.setString(std::to_string(FPSNumFrames));
            FPSUpdateTime -= sf::seconds(1.0f);
            FPSNumFrames = 0;
        }
    }
    void CellSim::render() noexcept{
        win.clear(sf::Color::Black);
        for(auto i=0u;i<(lastAlive +1); ++i){
            cells[i].render(win, opts.renderSensorPoints);
            if(opts.renderInfo){
                cells[i].renderInfoBars(win);
            }
            if(opts.renderBrainActivity){
                auto ba = getBrainActivity(cells[i]);
                win.draw(ba);
            }
        }
        auto SIMView = win.getView();
        auto GUIView = win.getDefaultView();
        win.setView(GUIView);
        win.draw(FPSText);
        win.draw(sizeText);
        win.draw(genInfo);
        win.setView(SIMView);
        win.display();
    }
    const sf::Time CellSim::TimePerFrame = sf::seconds(1.f/60.f);
}
int main(int argc, const char** argv){
    auto opts = EvoAI::parseOptions(argc, argv);
    if(opts.help){
        EvoAI::CellSim::usage();
        return 0;
    }
    EvoAI::randomGen().setSeed(opts.seed);
    if(!opts.loadFilename.empty()){
        JsonBox::Value v;
        v.loadFromFile(opts.loadFilename);
        if(v["CellSim"].isNull()){
            std::cerr << opts.loadFilename << " is not a valid CellSim json file." << std::endl;
            return EXIT_FAILURE;
        }
        EvoAI::CellSim app(v["CellSim"].getObject());
        app.run();
    }else{
        EvoAI::CellSim app(opts);
        app.run();
    }
    return 0;
}
