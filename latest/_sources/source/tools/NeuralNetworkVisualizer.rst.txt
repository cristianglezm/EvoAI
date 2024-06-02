Neural Network Visualizer
=============================

This tool will show you Neural networks and convert them to dot files.

Use NeuralNetworkVisualizer irisModel.json --dot to check irisModel and produce irisModel.dot
then use dot -Tpng -o irisModel.png to produce an image with dot.

.. code-block:: bash

    usage:
    NeuralNetworkVisualizer <filename> --dot

Keys
--------

* Add(\+)       \- zoom in
* Subtract(\-)  \- zoom out
* Down          \- it will move screen down
* Up            \- it will move screen up
* Left          \- it will move screen left
* Right         \- it will move screen right
* T             \- it will show text
