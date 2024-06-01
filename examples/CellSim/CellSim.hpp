#ifndef EVOAI_CELL_SIM_HPP
#define EVOAI_CELL_SIM_HPP

#include <SFML/Graphics.hpp>

#include <EvoAI.hpp>
#include "Tournament.hpp"
#include "Options.hpp"
#include "Averages.hpp"
#include "Cell.hpp"
#include "Trainer.hpp"
#include "Timer.hpp"

#include <stack>

namespace EvoAI{
	/**
	 * @brief CellSim App example
	 */
	class CellSim final{
        public:
            using Clock = std::chrono::system_clock;
		public:
            /**
             * @brief constructor
             * @param opts 
             */
            CellSim(const Options& opts);
            /**
             * @brief loads a
             * @param o JsonBox::Object
             */
            CellSim(JsonBox::Object o);
            /**
             * @brief converts CellSim to JsonBox::Value
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief write the simulation into a file.
             * @param filename 
             */
            void writeToFile(std::string_view filename) const noexcept;
            /**
             * @brief run the app
             */
            void run() noexcept;
            /**
             * @brief help aka usage for the cli
             */
            static void usage() noexcept;
        private: // population management
            /**
             * @brief static member func to make ids for cells.
             */
            static inline std::size_t genID() noexcept;
            /**
             * @brief Fn to give to Population, manages the creation of cells
             * @return Cell* 
             */
            Cell* createCells() noexcept;
            /**
             * @brief replaces the cells that are dead with the new generation.
             * @param toReplace std::vector<Cell*>&
             * @param toAdd std::vector<Cell>&
             */
            void replace(std::vector<Cell*>& toReplace, std::vector<Cell>& toAdd) noexcept;
            /**
             * @brief removes the species that are stagnant
             * @param ids std::vector<std::size_t>&&
             */
            void removeCellsFromSpecies(std::vector<std::size_t>&& ids) noexcept;
            /**
             * @brief uses SelectionAlgorithms::Tournament<Cell*> to reproduce 
             * the best and substitute the dead cells
             */
            void nextGeneration() noexcept;
        private:
            /**
             * @brief configure GUI. 
             */
            void setupGUI() noexcept;
            /**
             * @brief makes a visualization of brain activity.
             * @return sf::VertexArray
             */
            sf::VertexArray getBrainActivity(Cell& cell) noexcept;
            /**
             * @brief makes a Genome with a hybrid topology for Cells.
             * @return Genome
             */
            Genome makeCellGenome() noexcept;
            /**
             * @brief checks collisions
             * @todo add a quadtree if you want to be able to handle more cells
             * @param c1 Cell&
             * @param c2 Cell&
             */
            void checkCollision(Cell& c1, Cell& c2) noexcept;
            /**
             * @brief fixes the position of the cell inside to keep inside the screen.
             * @param c Cell&
             */
            void checkBounds(Cell& c) noexcept;
        private:
            void handleInput(sf::Event& e) noexcept;
            void update(sf::Time dt) noexcept;
            void updateFPS(sf::Time dt) noexcept;
            void render() noexcept;
        private:
            Options opts;
            sf::FloatRect bounds;
            sf::RenderWindow win;
            std::vector<Cell> cells;
            sf::Font FPSFont;
            sf::Text FPSText;
            sf::Text sizeText;
            sf::Text genInfo;
            std::unique_ptr<Population<Cell*>> pop;
            std::unique_ptr<Averages> avgs;
            std::size_t FPSNumFrames;
            std::size_t gen;
            std::size_t lastAlive;
            sf::Time FPSUpdateTime;
            Timer nextGenTimer;
            Trainer trainer;
            static const sf::Time TimePerFrame;
	};
    std::size_t CellSim::genID() noexcept{
        static std::size_t id = 0u;
        return id++;
    }
}

#endif // EVOAI_CELL_SIM_HPP