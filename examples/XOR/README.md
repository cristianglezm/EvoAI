# XOR

This example will train or evolve(using NEAT or HyperNEAT) a neural network to solve XOR.

```bash
Usage: XOR <mode>
                -e, --evolve <hn>                    Tries to solve XOR evolving a population to solve the XOR.(hn use HyperNeat instead of NEAT)
                -t, --train                          Trains a neural network to solve the XOR.
                -c, --check <g|n> <filename>            check a genome or a neural network.
                -s, --save-nn <filename>                Saves the neural network.
                -sg, --save-g <filename>                Saves the genome.
                -h, --help                              This menu.
```