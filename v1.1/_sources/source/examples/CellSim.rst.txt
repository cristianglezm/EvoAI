.. default-domain:: cpp
.. highlight:: cpp

CellSim
========

CellSim is a SFML app in which some basic shapes will evolve behaviors to hunt or scavenger food(other cells).

This example uses an Observer :class:`Population<EvoAI::Population>` meaning that the :class:`Population<EvoAI::Population>` 
is not the owner of the data, you use the observer Population when you give it a T* as the template parameter.

Cell
-----

The Cell class needs to fulfill :func:`meta::is_populable_v\<Cell\><EvoAI::meta::is_populable_v>` to be able to use it in :class:`Population<EvoAI::Population>`
``reproduce`` and ``distance`` are static methods.

The ``Cell`` also needs to have a ``constructor`` that takes a :class:`Genome<EvoAI::Genome>` and 
a :class:`EvoVector\<std::uint8_t\><EvoAI::EvoVector>` to be able to have children from a pair of Cells.

.. code-block:: cpp

    Cell(JsonBox::Object o) noexcept{
        // omitted
    }
    JsonBox::Value toJson() noexcept{
        // ommited    
    }
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
    // more code
    Cell Cell::reproduce(const Cell& cell1, const Cell& cell2) noexcept{
        return Cell(Genome::reproduce(cell1.genome, cell2.genome), EvoVector<std::uint8_t>::reproduce(cell1.evoVector, cell2.evoVector));
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

Population Management
----------------------

The population Management consist on creating and replacing those that died with new cells from the best of the population
in which we use an specialized :class:`Tournament<EvoAI::SelectionAlgorithms::Tournament>` to select the best and set the losers from the dead cells.

CreateCells
^^^^^^^^^^^^

When first creating the :class:`Population\<Cell*\><EvoAI::Population>` we will need to create the initial population
which is handled in the if condition, the else condition will handle the ``replacement`` of dead cells with new cells.

The method will return a ``Cell*`` which then :class:`Population<EvoAI::Population>` will use :func:`addMember<EvoAI::Population::addMember>` to 
add it to the :class:`Population<EvoAI::Population>` and assign or create its own :class:`Species<EvoAI::Species>`.

.. code-block:: cpp

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

Replace
^^^^^^^^

This method is responsable for taking the ``toReplace`` and ``toAdd`` vectors from :func:`Population::reproduce<EvoAI::Population::reproduce>` and
process them.

the ``if(toReplace.empty())`` is to handle :func:`Population::regrowPopulationFromElites<EvoAI::Population::regrowPopulationFromElites>` in case we want to call it 
instead of :func:`Population::regrowPopulation<EvoAI::Population::regrowPopulation>`.

The ``else if`` is the most likely path to be executed as :class:`Tournament<EvoAI::SelectionAlgorithms::Tournament>` will always return an empty selection 
if there are not dead cells.

We call :func:`Population::removeMember<EvoAI::Population::removeMember>` to remove the dead cell from :class:`Population\<Cell*\><EvoAI::Population>` 
and his :class:`Species<EvoAI::Species>` if is the last member alive, then we will call :func:`Population::addMember<EvoAI::Population::addMember>` 
for the new ``Cell`` to be added or assigned a new :class:`Species<EvoAI::Species>`.

.. code-block:: cpp

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

removeCellsFromSpecies
^^^^^^^^^^^^^^^^^^^^^^^

This method will be responsable to kill the cells who species is over ``maxAge`` set in :class:`Population<EvoAI::Population>` after we call
:func:`Population::increaseAgeAndRemoveOldSpecies<EvoAI::Population::increaseAgeAndRemoveOldSpecies>`, we call this function to increase the age of
the :class:`Species<EvoAI::Species>` and if their avg Fitness is stagnant(it doesn't get better) their age is increased x2 and is sooner removed from the
:class:`Population<EvoAI::Population>`.

.. code-block:: cpp

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

nextGeneration
^^^^^^^^^^^^^^^

This method will advance to the next generation, we use an ``specialized Tournament`` (we show it on the next section).
When we call :func:`Population::reproduce<EvoAI::Population::reproduce>` it will return a pair of two vectors, one ``std::vector<Cell*>`` and a ``std::vector<Cell>``
the first one are the dead cells and the second the children from the best cells.

.. code-block:: cpp

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

Tournament specialization
^^^^^^^^^^^^^^^^^^^^^^^^^^

As said before the :class:`Tournament<EvoAI::Tournament>` is specialized to ``Cell*``

This code is the one for interspecies reproduction

.. code:: cpp

    template<>
    std::vector<Selected<Cell*>> Tournament<Cell*>::operator()(std::vector<std::remove_pointer_t<Cell*>*>& members, [[maybe_unused]] std::size_t numberToSelect) noexcept{
        std::vector<Selected<Cell*>> selected;
        std::vector<pointer> livingCells;
        livingCells.reserve(members.size());
        std::queue<pointer> deadCells;
        for(auto& cell:members){
            if(cell->isAlive()){
                livingCells.emplace_back(cell);
            }else if(!cell->canBeEaten()){
                deadCells.emplace(cell);
            }
        }
        if((members.size() == livingCells.size()) || livingCells.empty()){
            return selected;
        }
        selected.reserve(deadCells.size());
        while(!deadCells.empty()){
            auto father = fight(livingCells);
            auto mother = fight(livingCells);
            if(father.first == mother.first){
                mother = fight(livingCells);
            }
            auto cell = deadCells.front();
            deadCells.pop();
            selected.emplace_back(father.first, mother.first, cell);
        }
        return selected;
    }

This code is the one for :class:`Species<EvoAI::Species>` reproduction.

.. code-block:: cpp

    template<>
    std::vector<Selected<Cell*>> Tournament<Cell*>::operator()(std::map<std::size_t, std::unique_ptr<Species<Cell*>>>& species, std::size_t numberToSelect) noexcept{
        std::vector<Selected<Cell*>> selected;
        selected.reserve(numberToSelect);
        for(auto& [id, sp]: species){
            auto& members = sp->getMembers();
            std::vector<pointer> livingCells;
            livingCells.reserve(members.size());
            std::queue<pointer> deadCells;
            for(auto& cell:members){
                if(cell->isAlive()){
                    livingCells.emplace_back(cell);
                }else if(!cell->canBeEaten()){
                    deadCells.emplace(cell);
                }
            }
            if(livingCells.empty()){
                continue;
            }
            while(!deadCells.empty()){
                auto father = fight(livingCells);
                auto mother = fight(livingCells);
                if(father.first == mother.first){
                    mother = fight(livingCells);
                }
                auto cell = deadCells.front();
                deadCells.pop();
                selected.emplace_back(father.first, mother.first, cell);
            }
        }
        return selected;
    }

the full code is `here`_

.. _here: https://github.com/cristianglezm/EvoAI/blob/master/examples/CellSim/