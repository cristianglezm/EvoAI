.. default-domain:: cpp
.. highlight:: cpp

NumClassifier
=============

In this example we will train a number classifier.

NumberClassDataset
------------------

we will create a Dataset to generate the data and labels, NumberClassDataset will need to fulfill :func:`meta::is_a_dataset_v\<NumberClassDataset\><EvoAI::meta::is_a_dataset_v>`
which just asserts that ``NumberClassDataset`` is a :class:`Dataset<EvoAI::Dataset>`

.. code-block:: cpp
    
    struct NumberClassDataset{
        NumberClassDataset(std::size_t samples, std::size_t batchSize)
        : m_index(0)
        , m_samples(samples)
        , m_batchSize(batchSize)
        , m_dataset(make_dataset(samples)){}
        std::vector<std::pair<std::vector<double>, std::vector<double>>> make_dataset(std::size_t samples) noexcept{
            // full code at the bottom
        }
        const std::pair<std::vector<double>&, std::vector<double>&> operator()() noexcept{
            auto i = m_index;
            m_index = (m_index + 1) % m_samples;
            return std::make_pair(std::ref(m_dataset[i].first), std::ref(m_dataset[i].second));
        }
        std::size_t size() const noexcept{
            return (m_dataset.size() + m_batchSize - 1) / m_batchSize;
        }
        std::size_t getBatchSize() const noexcept{
            return m_batchSize;
        }
        void shuffle() noexcept{
            auto g = EvoAI::randomGen().getEngine();
            std::uniform_int_distribution ud(0, static_cast<int>(m_dataset.size() - 1));
            for(auto i=0u;i<m_dataset.size();++i){
                auto index1 = ud(g);
                auto index2 = ud(g);
                std::swap(m_dataset[index1], m_dataset[index2]);
            }
        }
        // data
        mutable std::size_t m_index;
        std::size_t m_samples;
        std::size_t m_batchSize;
        std::vector<std::pair<std::vector<double>, std::vector<double>>> m_dataset;
    };

testNN function
---------------

This function will be passed to :class:`nn-\>train<EvoAI::NeuralNetwork::train>` to check how well is doing with the test DataSet

.. code-block:: cpp

    std::pair<double, double> testNN(NeuralNetwork& nn, DataLoader<NumberClassDataset>& ds) noexcept{
        double error = 0.0;
        double totalLoss = 0.0;
        for(auto i=0u;i<ds.size();++i){
            for(auto j=0u;j<ds.getBatchSize();++j){
                auto [inputs, expectedOutputs] = ds();
                auto outputs = nn.forward(inputs);
                nn.reset();
                totalLoss += Loss::MultiClassCrossEntropy{}(expectedOutputs,outputs);
                auto pred = Argmax(outputs);
                if(pred != Argmax(expectedOutputs)){
                    error += 1.0;
                }
                auto isCorrect = pred == Argmax(expectedOutputs) ? "v/":"X";
                std::cout << "input -> " << inputs[0] << " :: " << (pred+1) <<" = " << (Argmax(expectedOutputs)+1) <<"\t" << isCorrect << "\n";
            }
        }
        auto avgLoss = totalLoss / (ds.size() * ds.getBatchSize());
        error /= (ds.size() * ds.getBatchSize());
        error *= 100;
        std::cout << " error: " << error  << "% " << "accuracy: " << (100.0 - error)  << "%" << std::endl;
        std::cout << " avgLoss: " << avgLoss << std::endl; 
        return std::make_pair(avgLoss, 100.0 - error);
    }

main
-----

In the main function we will call createFeedForwardNN to make a :class:`NeuralNetwork<EvoAI::NeuralNetwork>` then 
call :func:`UniformInit<EvoAI::UniformInit>` that will initialize the weights of the :class:`NeuralNetwork<EvoAI::NeuralNetwork>` 
and then set the actiovations for each layer.

We will generate the data (100 random numbers between 1 and 3), show an initial inference, configure the :class:`Optimizer<EvoAI::Optimizer>`
and then call :class:`NeuralNetwork::train<EvoAI::NeuralNetwork::train>`

.. code-block:: cpp

    int main(){
        EvoAI::randomGen().setSeed(42);
        auto nn = EvoAI::createFeedForwardNN(1,2,{6, 6},3, 1.0);
        EvoAI::UniformInit(*nn);
        (*nn)[1].setActivationType(EvoAI::Neuron::ActivationType::RELU);
        (*nn)[2].setActivationType(EvoAI::Neuron::ActivationType::IDENTITY);
        (*nn)[3].setActivationType(EvoAI::Neuron::ActivationType::SOFTMAX);
        std::cout << "before Training:"<< std::endl;
        auto batchSize = 10u;
        EvoAI::DataLoader<EvoAI::NumberClassDataset> trainingData(EvoAI::NumberClassDataset(100, batchSize));
        EvoAI::DataLoader<EvoAI::NumberClassDataset> testingData(EvoAI::NumberClassDataset(10, batchSize));
        testNN(*nn, testingData);
        EvoAI::Optimizer optim(0.1, batchSize, EvoAI::SGD(nn->getParameters(), 0.0), EvoAI::Scheduler(EvoAI::MultiStepLR({25}, 0.1)));
        EvoAI::writeMultiPlot("NumbersClassLossPlot.txt", {"epochAvgLoss", "testAvgLoss", "accuracy"}, 
                        nn->train(trainingData, testingData, optim, 100, EvoAI::Loss::MultiClassCrossEntropy{}, &EvoAI::testNN));
        nn->writeDotFile("numClass.dot");
        std::cout << "after Training:"<< std::endl;
        testNN(*nn, testingData);
        nn->writeToFile("NumberClass.json");
        return 0;
    }

the full code is `here`_

.. _here: https://github.com/cristianglezm/EvoAI/blob/master/examples/NumClassifier/NumClassifier.cpp
