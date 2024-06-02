.. default-domain:: cpp
.. highlight:: cpp

Examples
========

A simple example where we evolve an EvoString to the objective word
in this case is "Hello world!"

.. code-block:: cpp

    std::string word("hello world!");
    auto numPop = 500u;
    // create a Population of EvoString, 
    // the first arg is the population, 
    // the next three are the coefficients and the last is the EvoString argument
    EvoAI::Population<EvoAI::EvoString> pop(numPop,2.0,2.0,1.0, word.size());
    auto correct = 0u;
    // lambda to pass to Population::eval
    auto eval = [&word, &correct](auto& w){
        correct = 0u;
        w.mutate();
        for(auto i=0u;i<word.size();++i){
            if(word[i] == w[i]){
                correct += 1;
            }
        }
        w.setFitness(correct);
    };
    auto interspecies = true;
    do{
        pop.eval(eval);
        if(correct != word.size()){
            pop.reproduce(EvoAI::SelectionAlgorithms::Tournament<EvoAI::EvoString>{numPop}, interspecies);
            pop.increaseAgeAndRemoveOldSpecies();
            pop.regrowPopulation(2.0, 2.0, 1.0, word.size());
        }
    }while(correct != word.size());
    auto best = pop.getBestMember();
    if(best){
        std::cout << "The best is ";
        for(auto i=0u;i<best->size();++i){
            std::cout << (*best)[i];
        }
        std::cout << '\0' << std::endl;
    }

you can see this examples in `EvoVectorTest`_ or execute it running

.. code-block:: bash

    bin/RunAllTests --gtest_filter=EvoVectorTest.EvolveWord

.. _EvoVectorTest: https://github.com/cristianglezm/EvoAI/blob/master/test/EvoVectorTest.hpp

more complete examples are at:

.. toctree::
   :maxdepth: 1

   IrisClassification <examples/IrisClassification>
   CellSim <examples/CellSim>
   NumClassifier <examples/NumClassifier>
   XOR <examples/XOR>