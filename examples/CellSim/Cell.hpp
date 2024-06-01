#ifndef EVOAI_CELL_HPP
#define EVOAI_CELL_HPP

#include <EvoAI/EvoVector.hpp>
#include <EvoAI/Genome.hpp>

#include <array>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "InfoBar.hpp"
#include "Timer.hpp"

namespace EvoAI{
    inline constexpr double magnitude(const sf::Vector2f& v) noexcept;
    inline sf::Vector2f normalize(const sf::Vector2f& v) noexcept;
    inline constexpr double degreesToRadians(float d) noexcept;
    inline constexpr double radiansToDegrees(float r) noexcept;
    /**
     *  @brief get the difference between v and origin in radians
     *  clockwise direction and -y is 0º degrees
     *  @code
     *      // angle is 281.31º from 0,0 to 5,1 in clockwise direction
     *      auto angle = Math::radiansToDegrees(Math::angle({0,0},{5,1}));
     *      // angle is 101.31º from 5,1 to 0,0 in clockwise direction
     *      angle = Math::radiansToDegrees(Math::angle({5,1},{0,0}));
     *      //
     *  @endcode
     *  @param v vector
     *  @param origin Origin which of vector v
     *  @return double angle in radians
     */
    constexpr double angle(const sf::Vector2f& v, const sf::Vector2f& origin = sf::Vector2f(0.0, 0.0)) noexcept;
    /**
     * @brief Stats for Cells
     * @param .atkPower
     * @param .stamina
     * @param .health
     * @param .defense
     * @param .nutrients
     * @param .hunger
     */
    struct Stats final{
        std::uint8_t atkPower;
        std::uint8_t stamina;
        std::uint8_t health;
        std::uint8_t defense;
        std::uint8_t nutrients;
        std::uint8_t hunger;
    };
    class Cell final{
        private:
            /**
             * @brief enum for EvoVector[] access
             */
            enum esAttr{
                BodyType = 0,
                FillColorR,
                FillColorG,
                FillColorB,
                OutColorR,
                OutColorG,
                OutColorB,
                AtkPower,
                Stamina,
                Health,
                Defense,
                Nutrients,
                Hunger
            };
            /**
             * @brief enum for state output access
             */
            enum Actions{
                Idle = 0,
                MoveNorth,
                MoveNorthEast,
                MoveEast,
                MoveSouthEast,
                MoveSouth,
                MoveSouthWest,
                MoveWest,
                MoveNorthWest,
                Attacking,
                Eating,
                Defending
            };
        public:
            /**
             * @brief default constructor
             */
            Cell() noexcept;
            /**
             * @brief consturctor for trained cells.
             * @param g EvoAI::Genome&& 
             */
            Cell(Genome&& g) noexcept;
            /**
             * @brief constructor for children cells
             * @param g 
             * @param es 
             */
            Cell(Genome&& g, EvoVector<std::uint8_t>&& es) noexcept;
			/**
			 * @brief constructor for JsonBox::Object
			 * @param o JsonBox::Object
			 */
			Cell(JsonBox::Object o) noexcept;
			/**
			 * @brief convert to json
			 * @return JsonBox::Value
			 */
			JsonBox::Value toJson() const noexcept;
        public: // EvoAI::meta::is_Populable_v<Cell> fn
            /**
             * @brief sets the Cell id
             * @param ID std::size_t
             */
            inline void setID(std::size_t ID) noexcept;
            /**
             * @brief gets the Cell id
             * @return std::size_t
             */
            inline std::size_t getID() const noexcept;
            /**
             * @brief setter for species ID
             * @param spcID species id
             */
            inline void setSpeciesID(std::size_t spcID) noexcept;
            /**
             * @brief sets the fitness
             * @param fit double
             */
            inline void setFitness(double fit) noexcept;
            /**
             * @brief adds the amount to the current fitness.
             * @param amount double
             */
            inline void addFitness(double amount) noexcept;
            /**
             * @brief gets the fitness
             * @return double
             */
            inline double getFitness() const noexcept;
            /**
             * @brief returns the species id.
             * @return std::size_t&
             */
            inline std::size_t getSpeciesID() const noexcept;
        public:
            inline void setPosition(float x, float y) noexcept;
            inline void setPosition(sf::Vector2f pos) noexcept;
            inline sf::Vector2f getPosition() const noexcept;
            inline void moveNorth() noexcept;
            inline void moveNorthEast() noexcept;
            inline void moveEast() noexcept;
            inline void moveSouthEast() noexcept;
            inline void moveSouth() noexcept;
            inline void moveSouthWest() noexcept;
            inline void moveWest() noexcept;
            inline void moveNorthWest() noexcept;
            /**
             * @brief sets sensors bits
             * @param bitPos int
             * @param OnOrOff bool
             */
            inline void setSensorAt(int bitPos, bool OnOrOff) noexcept;
            /**
             * @brief gets sensor state at bitPos
             * @param bitPos int
             * @return bool
             */
            inline bool getSensorAt(int bitPos) noexcept;
            /**
             * @brief gets bodyType 0-5
             * @return int
             */
            inline int getBodyType() const noexcept;
            inline bool isAlive() const noexcept;
            inline void setAlive(bool alv) noexcept;
            inline bool isAttacking() const noexcept;
            inline bool isEating() const noexcept;
            inline bool isDefending() const noexcept;
            inline sf::FloatRect getBounds() const noexcept;
            inline const Stats& getMaxStats() noexcept;
            inline NeuralNetwork& getBrain() noexcept;
            /**
             * @brief gets the sensors points to check which side is colliding
             * @return std::vector<sf::Vector2f> points
             */
            inline std::vector<sf::Vector2f> getSensorsPoints() const noexcept;
            inline void decreaseHealth() noexcept;
            inline void increaseHealth() noexcept;
            inline void decreaseStamina() noexcept;
            inline void increaseStamina() noexcept;
            inline void decreaseNutrients() noexcept;
            inline void decreaseHunger() noexcept;
            inline bool canBeEaten() const noexcept;
            inline void setTarget(Cell* c) noexcept;
            inline void takeDamage(std::uint8_t amount) noexcept;
            inline std::uint8_t takeBite(std::uint8_t amount) noexcept;
            inline void heal(std::uint8_t amount) noexcept;
            inline void eat(std::uint8_t amount) noexcept;
            /**
             * @brief get the state and actions from cell.
             * @return std::pair<std::vector<double>, std::vector<double>>
             */
            inline std::pair<std::vector<double>, std::vector<double>> getSample() noexcept;
        public:
            void update(sf::Time dt) noexcept;
            void renderInfoBars(sf::RenderWindow& win) noexcept;
            void render(sf::RenderWindow& win, bool renderSensorPoints = false) noexcept;
		public: // EvoAI::meta::is_Populable_v<Cell> fn
			/**
			 * @brief reproduce the Cell
			 * @param cell1 const Cell&
			 * @param cell2 const Cell&
			 * @return Cell 
			 */
			static Cell reproduce(const Cell& cell1, const Cell& cell2) noexcept;
            /**
             * @brief Calculates the distance between two Cell.
             * @param cell1 const Cell&
             * @param cell2 const Cell&
             * @param c1 coefficient Gives importance to genome
             * @param c2 coefficient Gives importance to evoVector
             * @param c3 coefficient Gives importance to [not used]
             * @return double
             */
            static double distance(const Cell& cell1, const Cell& cell2, 
					[[maybe_unused]] double c1 = 0.01, 
					[[maybe_unused]] double c2 = 0.01, 
					[[maybe_unused]] double c3 = 1.0) noexcept;
        private:
            void init() noexcept;
        private:
            Genome genome;
            EvoVector<std::uint8_t> evoVector;
            Stats maxStats;
            Stats currentStats;
            std::array<InfoBar, 4> infoBars;
            std::vector<double> state;
            std::vector<double> actions;
            std::unique_ptr<sf::Shape> body;
            std::unique_ptr<NeuralNetwork> brain;
            Cell* target;
            Timer hungerTimer;
            Timer deadTimer;
            double speed;
            std::size_t lastAction;
            std::uint8_t sensors;
            bool attacking;
            bool eating;
            bool defending;
            bool alive;
    };
// math helpers
    inline constexpr double magnitude(const sf::Vector2f& v) noexcept{
        return std::sqrt((v.x * v.x) + (v.y * v.y));
    }
    inline sf::Vector2f normalize(const sf::Vector2f& v) noexcept{
        auto m = magnitude(v);
        if(m > 0.0){
            return sf::Vector2f(v.x/m, v.y/m);
        }
        return sf::Vector2f();
    }
    inline constexpr double degreesToRadians(float d) noexcept{
        return (d * PI/180.0);
    }
    inline constexpr double radiansToDegrees(float r) noexcept{
        return (r * 180.0/PI);
    }
    constexpr double angle(const sf::Vector2f& v, const sf::Vector2f& origin) noexcept{
        auto angle = std::atan2((v.y - origin.y), (v.x - origin.x)) + degreesToRadians(90);
        if(angle < 0){
            angle += 2 * PI;
        }
        if(angle > 2.0 * PI){
            angle -= 2.0 * PI;
        }
        return angle;
    }
// inlined impl
	void Cell::setID(std::size_t ID) noexcept{
		evoVector.setID(ID);
        genome.setID(ID);
	}
	std::size_t Cell::getID() const noexcept{
		return evoVector.getID();
	}
	void Cell::setSpeciesID(std::size_t spcID) noexcept{
		evoVector.setSpeciesID(spcID);
        genome.setSpeciesID(spcID);
	}
	void Cell::setFitness(double fit) noexcept{
		evoVector.setFitness(fit);
        genome.setFitness(fit);
	}
	void Cell::addFitness(double amount) noexcept{
        evoVector.addFitness(amount);
        genome.addFitness(amount);
	}
	double Cell::getFitness() const noexcept{
		return evoVector.getFitness();
	}
	std::size_t Cell::getSpeciesID() const noexcept{
		return evoVector.getSpeciesID();;
	}
    const Stats& Cell::getMaxStats() noexcept{
        return maxStats;
    }
// state
    void Cell::setPosition(float x, float y) noexcept{
        body->setPosition(x,y);
    }
    void Cell::setPosition(sf::Vector2f pos) noexcept{
        body->setPosition(pos.x,pos.y);
    }
    sf::Vector2f Cell::getPosition() const noexcept{
        return body->getPosition();
    }
    void Cell::moveNorth() noexcept{
        body->move(0.0, -1.0 * speed);
    }
    void Cell::moveNorthEast() noexcept{
        body->move(1.0 * speed, -1.0 * speed);
    }
    void Cell::moveEast() noexcept{
        body->move(1.0 * speed, 0.0);
    }
    void Cell::moveSouthEast() noexcept{
        body->move(1.0 * speed, 1.0 * speed);
    }
    void Cell::moveSouth() noexcept{
        body->move(0.0, 1.0 * speed);
    }
    void Cell::moveSouthWest() noexcept{
        body->move(-1.0 * speed, -1.0 * speed);
    }
    void Cell::moveWest() noexcept{
        body->move(-1.0 * speed, 0.0);
    }
    void Cell::moveNorthWest() noexcept{
        body->move(-1.0 * speed, -1.0 * speed);
    }
    void Cell::setSensorAt(int bitPos, bool OnOrOff) noexcept{
        if(OnOrOff){
            sensors |= 1 << bitPos;
        }else{
            sensors &= ~(1 << bitPos);
        }
    }
    bool Cell::getSensorAt(int bitPos) noexcept{
        return sensors & (1 << bitPos);
    }
    int Cell::getBodyType() const noexcept{
        return normalize<int>(evoVector[esAttr::BodyType], 0, 5, 0, 255);
    }
    bool Cell::isAlive() const noexcept{
        return alive;
    }
    void Cell::setAlive(bool alv) noexcept{
        if(alv){
            currentStats.health = maxStats.health;;
        }else{
            currentStats.health = 0;
        }
        alive = alv;
    }
    bool Cell::isAttacking() const noexcept{
        return attacking;
    }
    bool Cell::isEating() const noexcept{
        return eating;
    }
    bool Cell::isDefending() const noexcept{
        return defending;
    }
    sf::FloatRect Cell::getBounds() const noexcept{
        return body->getGlobalBounds();
    }
    std::vector<sf::Vector2f> Cell::getSensorsPoints() const noexcept{
        std::vector<sf::Vector2f> points;
        points.reserve(body->getPointCount());
        for(auto i=0u;i<body->getPointCount();++i){
            auto pos = body->getPosition();
            points.emplace_back(pos + body->getPoint(i));
        }
        return points;
    }
    NeuralNetwork& Cell::getBrain() noexcept{
        return *brain;
    }
    void Cell::decreaseHealth() noexcept{
        if(currentStats.health > 0){
            --currentStats.health;
        }
    }
    void Cell::increaseHealth() noexcept{
        if(currentStats.health < maxStats.health){
            ++currentStats.health;
        }
    }
    void Cell::decreaseStamina() noexcept{
        if(currentStats.stamina > 0){
            --currentStats.stamina;
        }
    }
    void Cell::increaseStamina() noexcept{
        if(currentStats.stamina < maxStats.stamina){
            ++currentStats.stamina;
        }
    }
    void Cell::decreaseNutrients() noexcept{
        if(currentStats.nutrients > 0){
            --currentStats.nutrients;
        }
    }
    void Cell::decreaseHunger() noexcept{
        if(currentStats.hunger > 0){
            --currentStats.hunger;
        }
    }
    bool Cell::canBeEaten() const noexcept{
        return !alive && (currentStats.nutrients > 0);
    }
    void Cell::setTarget(Cell* c) noexcept{
        target = c;
    }
    void Cell::takeDamage(std::uint8_t amount) noexcept{
        std::uint8_t dmg = amount;
        if(currentStats.defense > 0 && defending){
            dmg = amount / currentStats.defense;
        }
        if(currentStats.health >= dmg){
            currentStats.health -= dmg;
        }else{
            currentStats.health = 0;
        }
    }
    std::uint8_t Cell::takeBite(std::uint8_t amount) noexcept{
        if(currentStats.nutrients >= amount){
            currentStats.nutrients -= amount;
            return amount;
        }else{
            auto ret = currentStats.nutrients;
            currentStats.nutrients = 0;
            return ret;
        }
    }
    void Cell::heal(std::uint8_t amount) noexcept{
        if((currentStats.health + amount) < maxStats.health){
            currentStats.health += amount;
        }else{
            currentStats.health = maxStats.health;
        }
    }
    void Cell::eat(std::uint8_t amount) noexcept{
        if((currentStats.hunger + amount) < maxStats.hunger){
            currentStats.hunger += amount;
        }else{
            currentStats.hunger = maxStats.hunger;
        }
    }
    std::pair<std::vector<double>, std::vector<double>> Cell::getSample() noexcept{
        return std::make_pair(state, actions);
    }
}

#endif // EVOAI_CELL_HPP
