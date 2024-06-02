.. default-domain:: cpp
.. highlight:: cpp

IrisClassification
==================

In this example we will build IrisClassification.

DataSet
-------

we will create a Dataset to load the data and labels, IrisDataset will need to fulfill :func:`meta::is_a_dataset_v\<IrisDataset\><EvoAI::meta::is_a_dataset_v>`
which just asserts that ``IrisDataset`` needs to have the following methods:

.. code-block:: cpp

    struct IrisDataset{
        using TrainingFormat = std::vector<std::pair<std::vector<double>,std::vector<double>>>;
        IrisDataset(std::string_view filename, bool train, std::size_t batchSize);
        const std::pair<std::vector<double>&, std::vector<double>&> operator()() noexcept;
        std::size_t size() const noexcept;
        std::size_t getBatchSize() const noexcept;
        void shuffle() noexcept;
        // data
        mutable std::size_t m_index;
        std::size_t m_batchSize;
        std::vector<std::pair<std::vector<double>,std::vector<double>>> m_data;
    };

the ``constructor`` will take care of loading the data, splitting it and one-hot encoding the target labels.

.. code-block:: cpp

    IrisDataset(std::string_view filename, bool train, std::size_t batchSize)
    : m_index(0u)
    , m_batchSize(batchSize){
        std::string dataInput(filename);
        std::fstream csv(dataInput);
        auto irisData = EvoAI::readCSVFile(csv);
        csv.close();
        std::mt19937_64 g = EvoAI::randomGen().getEngine();
        std::shuffle(std::begin(irisData), std::end(irisData), g);
        std::size_t start = 0u;
        auto percent = 1.0;
        if(train){
            percent = 0.8;
        }else{
            percent = 0.2;
        }
        std::size_t end = std::floor(irisData.size() * percent);
        for(auto i=start;i<end;++i){
            std::vector<double> in;
            in.reserve(irisData[i].size());
            std::vector<double> out;
            out.reserve(3);
            auto size = irisData[i].size()-1;
            for(auto j=0u;j<size;++j){
                in.emplace_back(std::stod(irisData[i][j]));
            }
            if(irisData[i][size] == "Iris-setosa"){
                out.emplace_back(1.0);
                out.emplace_back(0.0);
                out.emplace_back(0.0);
            }else if(irisData[i][size] == "Iris-versicolor"){
                out.emplace_back(0.0);
                out.emplace_back(1.0);
                out.emplace_back(0.0);
            }else if(irisData[i][size] == "Iris-virginica"){
                out.emplace_back(0.0);
                out.emplace_back(0.0);
                out.emplace_back(1.0);
            }
            m_data.emplace_back(in, out);
        }
    }

The ``size`` and ``getBatchSize`` will return the sizes

.. code-block:: cpp

    std::size_t size() const noexcept{
        return (m_data.size() + m_batchSize - 1) / m_batchSize;
    }
    std::size_t getBatchSize() const noexcept{
        return m_batchSize;
    }

the ``shuffle`` method will shuffle around the data so it doesn't repeat the same data for each batch.

.. code-block:: cpp

    void shuffle() noexcept{
        auto g = EvoAI::randomGen().getEngine();
        std::uniform_int_distribution ud(0, static_cast<int>(m_data.size() - 1));
        for(auto i=0u;i<m_data.size();++i){
            auto index1 = ud(g);
            auto index2 = ud(g);
            std::swap(m_data[index1], m_data[index2]);
        }
    }

the operator() will return an input and target to give to the :func:`NeuralNetwork::forward<EvoAI::NeuralNetwork::forward>` and 
:class:`LossFn<EvoAI::Loss::MultiClassCrossEntropy>`.

.. code-block:: cpp

    const std::pair<std::vector<double>&, std::vector<double>&> operator()() noexcept{
        auto i = m_index;
        m_index = (m_index + 1) % m_data.size();
        return std::make_pair(std::ref(m_data[i].first), std::ref(m_data[i].second));
    }

Training
--------

The training is just a call to :func:`nn-\>train<EvoAI::NeuralNetwork::train>` we use an :class:`Optimizer<EvoAI::Optimizer>` with a learning rate of 0.1, 
using :class:`SGD<EvoAI::SGD>` and a :class:`MultiStepLR<EvoAI::MultiStepLR>` which at step 175 will multiply the learning rate to 0.1, then we will write the data returned by
:func:`nn-\>train<EvoAI::NeuralNetwork::train>` to "irisAvgLoss.txt" so we can use ``python ..tools/showMultiLinePlot.py irisAvgLoss.txt``.

.. code-block:: cpp

    EvoAI::Optimizer optim(0.1, batchSize, EvoAI::SGD(m_nn->getParameters(), 0.0), EvoAI::Scheduler(EvoAI::MultiStepLR({175}, 0.1)));
    EvoAI::writeMultiPlot("irisAvgLoss.txt", {"epochAvgLoss", "testAvgLoss", "accuracy"},
        m_nn->train(trainingSet, testingSet, optim, epoch, EvoAI::Loss::MultiClassCrossEntropy{}, testDataset));

Evolve
--------

We will evolve a :class:`Population<EvoAI::Population>` of :class:`Genome<EvoAI::Genome>` to use NEAT, we will create a 
:class:`EvoAI::Population\<EvoAI::Genome\><EvoAI::Population>` of 500 members with coefficients of 2.0 for excess Genes, 2.0 for 
disjoints Genes and 1.0 for weight difference, the next two numbers are the :func:`Genome<EvoAI::Genome::Genome>` constructor 
which will make a :class:`Genome<EvoAI::Genome>` of 4 inputs and 3 outputs.

The :func:`Population::setCompatibilityThreshold<EvoAI::Population::setCompatibilityThreshold>` is set to 10, this is the :func:`Genome::distance<EvoAI::Genome::distance>` 
between genomes to be considered another :class:`Species<EvoAI::Species>`.

Then we will make a lambda to easily create the ``IrisClassifier`` and configure the ``ActivationType`` of the hidden and output layers.

.. code-block:: cpp

    EvoAI::Population<EvoAI::Genome> p(500, 2.0, 2.0, 1.0, 4,3);
    p.setCompatibilityThreshold(10.0);
    auto loss = 999.0;
    std::size_t gen = 0u;
    auto makeIrisClass = [&normalize](EvoAI::Genome& g){
        auto nn = EvoAI::Genome::makePhenotype(g);
        nn[1].setActivationType(EvoAI::Neuron::ActivationType::RELU);
        nn[2].setActivationType(EvoAI::Neuron::ActivationType::SOFTMAX);
        return std::make_unique<IrisClassifier>(normalize, std::make_unique<EvoAI::NeuralNetwork>(std::move(nn)));
    };

now we will make another lambda to evaluate the ``IrisClassifier`` from the given :class:`Genome<EvoAI::Genome>`.

We mutate the given :class:`Genome<EvoAI::Genome>` and then build the ``IrisClassifier`` to evaluate, we use the trainingSet 
to get input and target and call ``IrisClass->forward(input)`` add the loss and set the fitness of the given :class:`Genome<EvoAI::Genome>`

.. code-block:: cpp

    auto eval = [&](auto& g){
        loss = 0.0;
        g.mutate();
        auto irisClass = makeIrisClass(g);
        auto size = trainingSet.size() * trainingSet.getBatchSize();
        for(auto i=0u;i<size;++i){
            auto [input, target] = trainingSet();
            auto outputs = irisClass->forward(input);
            loss += EvoAI::Loss::MultiClassCrossEntropy{}(target, outputs);
        }
        loss /= size;
        g.setFitness(100.0 - loss);
    };

Then we will start the main loop to evaluate the members of the :class:`Population<EvoAI::Population>` and :func:`reproduce<EvoAI::Population::reproduce>` the best 
till the loss is less than ``lossThreshold`` which a 0.07 will give us a 89% accuracy.

.. code-block:: cpp

    while(loss > lossThreshold){
        p.eval(eval);
        std::cout << "\rGeneration: " << gen << " avg fitness: " << p.computeAvgFitness() << " NumSpecies: " << p.getSpeciesSize() << " Loss: " << loss << "       ";
        std::flush(std::cout);
        if(loss > lossThreshold){
            p.reproduce(EvoAI::SelectionAlgorithms::Tournament<EvoAI::Genome>{p.getPopulationMaxSize(), 5}, true);
            p.increaseAgeAndRemoveOldSpecies();
            p.regrowPopulation(2.0, 2.0, 1.0, 4, 3);
        }else{
            std::cout << std::endl;
        }
        ++gen;
    }
    auto g = p.getBestMember();
    g->writeToFile("irisGenNEAT.json");
    auto irisClass = makeIrisClass(*g);
    irisClass->writeToFile("irisNEAT.json");
    irisClass->writeDotFile("IrisNEAT.dot");
    irisClass->test(testingSet);


the full code is `here`_

.. _here: https://github.com/cristianglezm/EvoAI/blob/master/examples/IrisClassification/IrisClassification.cpp