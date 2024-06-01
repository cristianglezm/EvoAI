# Iris Classification

This example will train or evolve(NEAT or HyperNEAT) a Neural Network to classify the Iris Dataset.

```bash
usage: IrisClassification -df <dataInput> <options> [-m <filename>]
examples:
IrisClassification -df iris.data.csv -t // to train from zero
IrisClassification -df iris.data.csv -t -m irisModel.json // to keep training irisModel.json
V evolve a population till loss is under 0.07 if hn HyperNeat will be used.
IrisClassification -df iris.data.csv -e 0.07 [hn]
        -df <dataFilename>                      The file with the Iris Dataset.
        dataInput is a csv file with this format <sepal length>,<sepal width>,<petal length>,<petal width>,Iris-setosa || Iris-versicolor || Iris-virginica
        -m, --model <filename>                  Will load a IrisClassifier Model
                filename should be a json file of an IrisClassifier if empty it will create a random nn and save it as irisModel.json to train or test.
        -n, --norm                              Will normalize features of the data
        -t, --train                             Will train the network specified.
        -c, --classify                          Will test the network with the test data.
        -e, --evolve <lossThreshold> [hn]       Will evolve a population and select the best.
                                                [hn] will use hyperneat instead of neat
        -h, --help                              this menu.

```