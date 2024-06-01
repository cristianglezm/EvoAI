# Cell Sim

The cells indicate with the outline color what they are doing:

* Red - Attacking
* Green - Eating
* Blue - Defending

when renderSensorPoints is toggled

* Red - Inactive
* Green - Active

when renderBrainActivity is toggled

* Red - Inactive neuron
* Green - Active neuron

The species have an age limit of 20(default) generations (~10 if stagnant), 
the cells cannot attack or eat those from the same species, they get tired if they don't idle to 
restore stamina so the best will kind of shake as they move and idle pretty fast, the ones 
moving without idleing from time to time will exhaust themselves and die.

[![video](https://i.ytimg.com/vi/HV9gbBLeHVw/maxresdefault.jpg)](https://youtu.be/HV9gbBLeHVw)

The parameters for the video:

```bash
CellSim -mnc 200 -r 5 -is -ng 10 -sma 10 -numSamples 24 -epoch 15 -writeAvgs
```
## Usage

```bash
CellSim [options]
CellSim -is -mnc 100 -r 10 -ng 30 -c 0.01 0.01 1.0 -scr 1280 720 -seed 4242
CellSim -mnc 250 -r 5 -c 0.01 0.1 1.0 -scr 1920 1080 -seed 3253 -itc
        -is, --interspecies             if interspecies reproduction is allowed.
        -f, --font <TFF file>           TFF file to load.
        -mnc, --maxNumCell <n>          max number of cells.
        -r, --rounds <n>                Rounds for the Selection Algorithm (Tournament),
        -seed <n>                       seed for randomness.(default 42).
        -numSamples <n>                 number of samples.(default 20).
        -epoch <n>                      epoch for training.(default 10).
        -batchSize <n>                  Batch size.(default 12).
        -lr <float>                     learning rate (default 0.1)
        -train                          pretrain the cells with default values
        -ct <float>                     compatibility Threshold for speciation.
        -sma, --speciesMaxAge <n>               max Age for species
        -ng, --nextGen <seconds>                seconds to the next generation,
        -itc, --ignoreTimeCounter               ignore the time counter for next generation.
        -c, --coefficients <float float float>  importance of Genome, EvoString.
        -scr, --screen <width height>           screen resolution.
        -rsp, --renderSensorPoints              render points of contact
        -rba, --renderBrainActivity             render brain Activity
        -s, --save <filename>                   filename to save the simulation.
        -l, --load <filename>                   filename to load the simulation.
        -writeBB, --writeBestBrain              write the best cell::brain
        -writeAvgs, --writeAverages             write the averages of the Stats from Cells
        -h, --help                              This menu.
```

## Keys

* **Mouse Right-click** - trigger next Generation (it will do nothing if there are not dead cells)
* **Mouse Left-click** - kill population and regrow a new population
* **I** - display info bars
    + red - health
    + yellow - stamina
    + blue - hunger
    + green - nutrients
* **B** - display brain activity
    + red - inactive neuron
    + green - active neuron
* **S** - toggle interspecies
* **R** - toggle renderSensorPoints or figure
* **T** - train toggle (it will train the first generation)
* **P** - pause / unpause sim
* **ESC** - close app

## maybe TODO

* make a better Trainer::generateSample
* add samples from the best in the generation and train the new cells.
    + add a custom Dataset to be able to clear and add samples.
    + add a purge method to custom Dataset to clean samples where inputs and targets are all 0.
* add a player controllable cell and get the samples from it. 
    + use WASD to move, spacebar to attack, Q to defend, E to eat
