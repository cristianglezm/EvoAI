.. default-domain:: cpp
.. highlight:: cpp

XOR
=====

In this example we will train a :class:`NeuralNetwork<EvoAI::NeuralNetwork>` to solve XOR.

Dataset
--------

We will be using the class :class:`Dataset<EvoAI::Dataset>` as the XOR is a simple dataset.

.. code-block:: cpp

    std::vector<std::vector<double>> inputs;
    std::vector<std::vector<double>> outputs;
    {
        std::vector<double> x = {0.0,0.0,1.0,1.0};
        std::vector<double> y = {0.0,1.0,0.0,1.0};
        std::vector<double> truth = {0.0,1.0,1.0,0.0};
        for(auto i=0u;i<4;++i){
            std::vector<double> in;
            std::vector<double> out;
            in.emplace_back(x[i]);
            in.emplace_back(y[i]);
            out.emplace_back(truth[i]);
            inputs.emplace_back(in);
            outputs.emplace_back(out);
        }
    }
    // we need to make a copy for the test Dataset.
    auto tInputs = inputs;
    auto tOutputs = outputs;
    //...
    EvoAI::DataLoader trainDataset(EvoAI::Dataset(std::move(inputs), std::move(outputs), batchSize));
    EvoAI::DataLoader testDataset(EvoAI::Dataset(std::move(tInputs), std::move(tOutputs), batchSize));

Training
--------

we define trainXOR function that will take a :class:`NeuralNetwork<EvoAI::NeuralNetwork>` and two :class:`DataLoader<EvoAI::DataLoader>` 
one for trainingDataset and another for testDataset, then the epoch, batchSize and binaryCross to choose what Loss funtion to use.

.. code-block:: cpp

    void trainXOR(EvoAI::NeuralNetwork& nn, 
        EvoAI::DataLoader<EvoAI::Dataset>& trainDataset, 
        EvoAI::DataLoader<EvoAI::Dataset>& testDataset, std::size_t epoch, std::size_t batchSize, bool binaryCross){
        if(binaryCross){
            // Binary Cross Entropy
            EvoAI::Optimizer optim(0.9, batchSize, EvoAI::SGD(nn.getParameters(), 0.0), EvoAI::Scheduler(EvoAI::ConstantLR()));
            EvoAI::writeMultiPlot("xorAvgLoss.txt", {"epochAvgLoss", "testAvgLoss", "accuracy"},
                nn.train(trainDataset, testDataset, optim, epoch, EvoAI::Loss::BinaryCrossEntropy{}, &testXORDataset));
        }else{
            // Mean Squared Error
            EvoAI::Optimizer optim(0.5, batchSize, EvoAI::SGD(nn.getParameters(), 0.9), EvoAI::Scheduler(EvoAI::ConstantLR()));
            EvoAI::writeMultiPlot("xorAvgLoss.txt", {"epochAvgLoss", "testAvgLoss", "accuracy"},
                nn.train(trainDataset, testDataset, optim, epoch, EvoAI::Loss::MeanSquaredError{}, &testXORDataset));
        }
        nn.writeDotFile("xor.dot");
    }

Evolve
-------

We make a :class:`Population<EvoAI::Population>` of 500 members with :class:`Genomes<EvoAI::Genome>` of 2 inputs and 1 output.
we make a lambda to evaluate the :class:`Genomes<EvoAI::Genome>` and set the fitness, then, we do the 
main loop of eval, reproduce, age and kill stagnant species and call regrowPopulation to avoid going extinct.

Once the main loop is done, we get the :func:`Population::getBestMember<EvoAI::Population::getBestMember>` from the :class:`Population<EvoAI::Population>`.

.. code-block:: cpp

    void evolveNEAT(bool saveGen, const std::string& savingFileGenome, bool binaryCross) noexcept{
        std::cout << "Evolving NEAT Neural Networks..." << std::endl;
        EvoAI::Population<EvoAI::Genome> p(500,2.0, 2.0, 1.0, 2,1);
        p.setCompatibilityThreshold(10.0);
        std::vector<double> x = {0.0,0.0,1.0,1.0};
        std::vector<double> y = {0.0,1.0,0.0,1.0};
        std::vector<double> truth = {0.0,1.0,1.0,0.0};
        auto loss = 999.0;
        auto minError = 0.1;
        auto gen = 0u;
        auto eval = [&](auto& ge){
            loss = 0.0;
            std::vector<double> results;
            ge.mutate();
            auto phenotype = EvoAI::Genome::makePhenotype(ge);
            for(auto i=0u;i<4u;++i){
                auto out = phenotype.forward({x[i],y[i]});
                results.emplace_back(out[0]);
                phenotype.reset();
            }
            if(binaryCross){
                loss = EvoAI::Loss::BinaryCrossEntropy{}(truth, results);
            }else{
                loss = EvoAI::Loss::MeanSquaredError{}(truth, results);
            }
            ge.setFitness(100.0 - loss);
        };
        while(loss >= minError){
            p.eval(eval);
            std::cout << "\rGeneration: " << gen << " - AVG Fitness: " << p.computeAvgFitness() << " NumSpecies: " << p.getSpeciesSize() << " loss: " << loss << "\t\t\t";
            std::flush(std::cout);
            if(loss >= minError){
                p.reproduce(EvoAI::SelectionAlgorithms::Tournament<EvoAI::Genome>{p.getPopulationMaxSize(), 5}, true);
                p.increaseAgeAndRemoveOldSpecies();
                p.regrowPopulation(2.0, 2.0, 1.0, 2, 1);
                ++gen;
            }else{
                std::cout << std::endl;
            }
        }
        std::cout << "Selecting Winner..." << std::endl;
        auto ge = p.getBestMember();
        if(ge){
            if(saveGen){
                ge->writeToFile(savingFileGenome);
            }
            auto nn = EvoAI::Genome::makePhenotype(*ge);
            nn.writeDotFile("xorNEAT.dot");
            testXOR(nn);
        }
    }

the full code is `here`_

.. _here: https://github.com/cristianglezm/EvoAI/blob/master/examples/XOR/XOR.cpp