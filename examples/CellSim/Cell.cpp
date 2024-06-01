#include "Cell.hpp"
#include "Trainer.hpp"

namespace EvoAI{
    Cell::Cell() noexcept
    : genome(13u, 12u, true, false)
    , evoVector(13u)
    , maxStats()
    , currentStats()
    , infoBars{InfoBar(30.f, 5.f), InfoBar(30.f, 5.f), InfoBar(30.f, 5.f), InfoBar(30.f, 5.f)}
    , state(13, 0.0)
    , actions(12, 0.0)
    , body(nullptr)
    , brain(nullptr)
    , target(nullptr)
    , hungerTimer(std::chrono::milliseconds(150))
    , deadTimer(std::chrono::milliseconds(75))
    , speed(1.0)
    , lastAction(0)
    , sensors(0)
    , attacking(false)
    , eating(false)
    , defending(false)
    , alive(true){
        init();
    }
    Cell::Cell(Genome&& g) noexcept
    : genome(std::forward<Genome>(g))
    , evoVector(13u)
    , maxStats()
    , currentStats()
    , infoBars{InfoBar(30.f, 5.f), InfoBar(30.f, 5.f), InfoBar(30.f, 5.f), InfoBar(30.f, 5.f)}
    , state(13, 0.0)
    , actions(12, 0.0)
    , body(nullptr)
    , brain(nullptr)
    , target(nullptr)
    , hungerTimer(std::chrono::milliseconds(150))
    , deadTimer(std::chrono::milliseconds(75))
    , speed(1.0)
    , lastAction(0)
    , sensors(0)
    , attacking(false)
    , eating(false)
    , defending(false)
    , alive(true){
        init();
    }
    Cell::Cell(Genome&& g, EvoVector<std::uint8_t>&& es) noexcept
    : genome(std::forward<Genome>(g))
    , evoVector(std::forward<EvoVector<std::uint8_t>>(es))
    , maxStats()
    , currentStats()
    , infoBars{InfoBar(30.f, 5.f), InfoBar(30.f, 5.f), InfoBar(30.f, 5.f), InfoBar(30.f, 5.f)}
    , state(13, 0.0)
    , actions(12, 0.0)
    , body(nullptr)
    , brain(nullptr)
    , target(nullptr)
    , hungerTimer(std::chrono::milliseconds(150))
    , deadTimer(std::chrono::milliseconds(75))
    , speed(1.0)
    , lastAction(0)
    , sensors(0)
    , attacking(false)
    , eating(false)
    , defending(false)
    , alive(true){
        if(randomGen().random(0.4)){
            genome.mutateAddNode();
        }
        if(randomGen().random(0.4)){
            genome.mutateAddConnection();
        }
        if(randomGen().random(0.8)){
            genome.mutateWeights(2.0);
        }
        if(randomGen().random(0.3)){
            genome.mutateDisable();
        }else if(randomGen().random(0.3)){
            genome.mutateEnable();
        }
        evoVector.mutate();
        init();
    }
    Cell::Cell(JsonBox::Object o) noexcept
    : genome(o["Genome"].getObject())
    , evoVector(o["evoVector"].getObject())
    , maxStats()
    , currentStats()
    , infoBars{InfoBar(30.f, 5.f), InfoBar(30.f, 5.f), InfoBar(30.f, 5.f), InfoBar(30.f, 5.f)}
    , state(13, 0.0)
    , actions(12, 0.0)
    , body(nullptr)
    , brain(nullptr)
    , target(nullptr)
    , hungerTimer(std::chrono::milliseconds(150))
    , deadTimer(std::chrono::milliseconds(75))
    , speed(1.0)
    , lastAction(0)
    , sensors(0)
    , attacking(false)
    , eating(false)
    , defending(false)
    , alive(true){
        init();
    }
    JsonBox::Value Cell::toJson() const noexcept{
        JsonBox::Object o;
        o["Genome"] = genome.toJson();
        o["evoVector"] = evoVector.toJson();
        return o;
    }
    void Cell::init() noexcept{
        int bodyType = getBodyType();
        switch(bodyType){
            case 0: body = std::make_unique<sf::CircleShape>(20.0, 5);
                break;
            case 1: body = std::make_unique<sf::CircleShape>(20.0, 6);
                break;
            case 2: body = std::make_unique<sf::CircleShape>(20.0, 8);
                break;
            case 3: body = std::make_unique<sf::RectangleShape>(sf::Vector2f{20.0, 10.0});
                break;
            case 4: body = std::make_unique<sf::RectangleShape>(sf::Vector2f{20.0, 20.0});
                break;
            case 5:
            default: body = std::make_unique<sf::ConvexShape>(3);
                    sf::ConvexShape* cast = static_cast<sf::ConvexShape*>(body.get());
                    cast->setPoint(0, sf::Vector2f(0.f,0.f));
                    cast->setPoint(1, sf::Vector2f(20.f,20.f));
                    cast->setPoint(2, sf::Vector2f(-20.f,20.f));
                break;
        }
        auto fillColor = sf::Color(evoVector[esAttr::FillColorR], evoVector[esAttr::FillColorG], evoVector[esAttr::FillColorB]);
        auto outColor = sf::Color(evoVector[esAttr::OutColorR], evoVector[esAttr::OutColorG], evoVector[esAttr::OutColorB]);
        body->setFillColor(fillColor);
        body->setOutlineColor(outColor);
        maxStats = Stats{.atkPower = evoVector[esAttr::AtkPower], .stamina = evoVector[esAttr::Stamina], 
                            .health = evoVector[esAttr::Health], .defense = evoVector[esAttr::Defense], 
                            .nutrients = evoVector[esAttr::Nutrients], .hunger = evoVector[esAttr::Hunger]};
        if(maxStats.health == 0){
            maxStats.health = 1;
        }
        if(maxStats.stamina == 0){
            maxStats.stamina = 1;
        }
        if(maxStats.nutrients == 0){
            maxStats.nutrients = 1;
        }
        if(maxStats.atkPower == 0){
            maxStats.atkPower = 1;
        }
        if(maxStats.hunger == 0){
            maxStats.hunger = 1;
        }
        currentStats = maxStats;
        brain = std::make_unique<NeuralNetwork>(Genome::makePhenotype(genome));
        auto pos = getPosition();
        auto bounds = getBounds();
        auto size = infoBars[0].getSize();
        for(auto i=0u;i<infoBars.size();++i){
            infoBars[i].setBackgroundFillColor(sf::Color::White);
            switch(i){
                case 0: // health
                        infoBars[i].setFillColor(sf::Color::Red);
                    break;
                case 1: // stamina
                        infoBars[i].setFillColor(sf::Color::Yellow);
                    break;
                case 2: // hunger
                        infoBars[i].setFillColor(sf::Color::Blue);
                    break;
                case 3: // nutrients
                        infoBars[i].setFillColor(sf::Color::Green);
                    break;
            }
            infoBars[i].adjustAlphaChannel(200);
            infoBars[i].setPosition(pos + sf::Vector2f(0.0, bounds.height + (size.y * i) + 1.0));
        }
    }

    void Cell::update([[maybe_unused]] sf::Time dt) noexcept{
        // reset outline as indicator of actions
        body->setOutlineThickness(0.0);
        body->setOutlineColor(sf::Color::Transparent);
        attacking = false;
        eating = false;
        defending = false;
        if(isAlive()){
            auto targetAttacking = 0.0;
            auto targetAlive = 0.0;
            sensors = 0;
            if(target){
                if(target->getSpeciesID() != getSpeciesID()){
                    auto targetPos = target->getPosition();
                    auto points = getSensorsPoints();
                    for(auto& p:points){
                        auto degrees = radiansToDegrees(angle(normalize(targetPos - p)));
                        int index = static_cast<int>(std::round(degrees / 45.0)) % 8;
                        setSensorAt(index, true);
                    }
                    targetAttacking = (target->isAttacking() && target->isAlive()) ? 1.0:0.0;
                    targetAlive = target->isAlive() ? 1.0:0.0;
                }
            }
            // reset here so brain activity could be rendered after.
            brain->reset();
            state = {getSensorAt(0) ? 1.0:0.0, getSensorAt(1) ? 1.0:0.0, getSensorAt(2) ? 1.0:0.0, getSensorAt(3) ? 1.0:0.0, 
                                        getSensorAt(4) ? 1.0:0.0, getSensorAt(5) ? 1.0:0.0, getSensorAt(6) ? 1.0:0.0, getSensorAt(7) ? 1.0:0.0,
                                        static_cast<double>(currentStats.health / static_cast<double>(maxStats.health)), 
                                        static_cast<double>(currentStats.stamina / static_cast<double>(maxStats.stamina)), 
                                        static_cast<double>(currentStats.hunger / static_cast<double>(maxStats.hunger)),
                                        targetAttacking, targetAlive};
            actions = brain->forward(state);
            if(currentStats.stamina == 0){
                decreaseHealth();
            }else{
                decreaseStamina();
            }
            std::size_t currentAction = Argmax(actions);
            float decay = 1.0;
            if(lastAction == currentAction){
                decay = 0.01;
            }
            lastAction = currentAction;
            switch(currentAction){
                case Actions::Idle: //idle
                        // increase x2 stamina
                        increaseStamina();
                        increaseStamina();
                        if(currentStats.hunger < (maxStats.hunger * 0.5)){
                            addFitness(-1.0);
                        }else{
                            addFitness(0.1 * decay);
                        }
                        if(currentStats.stamina < (maxStats.stamina * 0.5)){
                            addFitness(1.0 * decay);
                        }else{
                            addFitness(0.1 * decay);
                        }
                    break;
                case Actions::MoveNorth:
                        moveNorth();
                        addFitness(1.0 * decay);
                        decreaseStamina();
                    break;
                case Actions::MoveNorthEast: 
                        moveNorthEast();
                        addFitness(1.0 * decay);
                        decreaseStamina();
                    break;
                case Actions::MoveEast: 
                        moveEast();
                        addFitness(1.0 * decay);
                        decreaseStamina();
                    break;
                case Actions::MoveSouthEast: 
                        moveSouthEast();
                        addFitness(1.0 * decay);
                        decreaseStamina();
                    break;
                case Actions::MoveSouth: 
                        moveSouth();
                        addFitness(1.0 * decay);
                        decreaseStamina();
                    break;
                case Actions::MoveSouthWest: 
                        moveSouthWest();
                        addFitness(1.0 * decay);
                        decreaseStamina();
                    break;
                case Actions::MoveWest: 
                        moveWest();
                        addFitness(1.0 * decay);
                        decreaseStamina();
                    break;
                case Actions::MoveNorthWest:
                        moveNorthWest();
                        addFitness(1.0 * decay);
                        decreaseStamina();
                    break;
                case Actions::Attacking: 
                        attacking = true;
                        body->setOutlineThickness(2.0);
                        body->setOutlineColor(sf::Color::Red);
                        if(target){
                            if(target->getSpeciesID() != getSpeciesID()){
                                target->takeDamage(currentStats.atkPower);
                                addFitness(10.0 * decay);
                                decreaseStamina();
                                decreaseStamina();
                            }
                        }else{
                            addFitness(-5.0);
                        }
                    break;
                case Actions::Eating:
                        eating = true;
                        body->setOutlineThickness(2.0);
                        body->setOutlineColor(sf::Color::Green);
                        if(target){
                            if((target->canBeEaten() && 
                                target->getSpeciesID() != getSpeciesID())){
                                    increaseStamina();
                                    auto amount = target->takeBite(currentStats.atkPower);
                                    heal(amount);
                                    eat(amount);
                                    addFitness(8.0 * decay);
                            }
                        }else{
                            addFitness(-5.0);
                        }
                        decreaseStamina();
                    break;
                case Actions::Defending:
                        defending = true;
                        body->setOutlineThickness(2.0);
                        body->setOutlineColor(sf::Color::Blue);
                        if(!target){
                            addFitness(-5.0);
                        }else{
                            addFitness(5.0 * decay);
                        }
                        decreaseStamina();
                    break;
            }
            target = nullptr;
            if(currentStats.health == 0){
                alive = false;
                auto c = body->getFillColor();
                c.a = 128;
                body->setFillColor(c);
            }
            if(currentStats.hunger == 0){
                decreaseHealth();
            }
            if(hungerTimer.update()){
                if(currentStats.stamina < (maxStats.stamina * 0.5)){
                    decreaseHunger();
                    decreaseHunger();
                }else{
                    decreaseHunger();
                }
            }
        }else{
            if(deadTimer.update()){
                decreaseNutrients();
            }
        }
        auto pos = body->getPosition();
        auto cBounds = getBounds();
        auto size = infoBars[0].getSize();
        for(auto i=0u;i<infoBars.size();++i){
            switch(i){
                case 0: // health
                        infoBars[i].updateInfo(normalize(static_cast<float>(currentStats.health), 0.f, size.x, 0.f, static_cast<float>(maxStats.health)));
                    break;
                case 1: // stamina
                        infoBars[i].updateInfo(normalize(static_cast<float>(currentStats.stamina), 0.f, size.x, 0.f, static_cast<float>(maxStats.stamina)));
                    break;
                case 2: // hunger
                        infoBars[i].updateInfo(normalize(static_cast<float>(currentStats.hunger), 0.f, size.x, 0.f, static_cast<float>(maxStats.hunger)));
                    break;
                case 3: // nutrients
                        infoBars[i].updateInfo(normalize(static_cast<float>(currentStats.nutrients), 0.f, size.x, 0.f, static_cast<float>(maxStats.nutrients)));
                    break;
            }
            infoBars[i].setPosition(pos + sf::Vector2f(0.0, cBounds.height + (size.y * i) + 1.0));
        }
    }
    void Cell::render(sf::RenderWindow& win, bool renderSensorPoints) noexcept{
        if(isAlive() || canBeEaten()){
            if(renderSensorPoints){
                auto points = getSensorsPoints();
                sf::VertexArray va;
                for(auto i=0u;i<points.size();++i){
                    sf::Color sensorStateColor = getSensorAt(i) ? sf::Color::Green : sf::Color::Red;
                    va.append(sf::Vertex(points[i], sensorStateColor));
                }
                va.setPrimitiveType(sf::PrimitiveType::Points);
                win.draw(va);
            }else{
                win.draw(*body);
            }
        }
    }
    void Cell::renderInfoBars(sf::RenderWindow& win) noexcept{
        if(isAlive() || canBeEaten()){
            for(auto& ib: infoBars){
                ib.render(win);
            }
        }
    }
    Cell Cell::reproduce(const Cell& cell1, const Cell& cell2) noexcept{
        return Cell(Genome::reproduce(cell1.genome, cell2.genome), 
                        EvoVector<std::uint8_t>::reproduce(cell1.evoVector, cell2.evoVector));
    }
    double Cell::distance(const Cell& cell1, const Cell& cell2, 
            [[maybe_unused]] double c1, 
            [[maybe_unused]] double c2, 
            [[maybe_unused]] double c3) noexcept{
                double gDistance = Genome::distance(cell1.genome, cell2.genome) * c1;
                double esDistance = cell1.getBodyType() != cell2.getBodyType() ? 5.0:0.0;
                for(auto i=1u;i<cell1.evoVector.size();++i){
                    esDistance += static_cast<double>(cell1.evoVector[i] != cell2.evoVector[i]) * c2;
                }
                return (gDistance + esDistance) / 2.0;
    }
}
