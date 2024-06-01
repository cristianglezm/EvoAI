# Image Generator

This tool will generate images.

## Examples

* This will make an image using coordinates 0..150 with a genome of type 0 (no hidden neurons), saving the genome file.

```bash
ImageGenerator -G 0 -C -f gimage1.png -res 150 150 -sg gimage1.json
```

* This will reproduce gimage1.json and gimage2.json to make an image(using coordinates) and a new genome gimage12.json(this is the offspring of gimage1 and gimage2).

```bash
ImageGenerator -g r gimage1.json gimage2.json -C -f gimage12.png -sg gimage12.json -res 150 150
```

* This will make an image using coordinates with a feedforward neural network with a hidden layer of 25 neurons.

```bash
ImageGenerator -N 1 1 25 -C -f nimage2.png -res 150 150 -s nimage2.json
```

## Tool help

```bash
ImageGenerator [options]
-g, --genome [m|r] <filename> [<filename> with r]       load a genome json file
                                        With m will mutate the genome.
                                        With r will combine two genomes, without m or r will load the genome.
-G, --genome-type <type> <numHidden>    will generate a genome of the type specified
                                                types:
                                                        0. Without hidden neurons
                                                        1. With hidden neurons.
-n, --neuralnetwork [g] <filename>              load a neural network json file.
                                                        if g is specified will make a genome from this network.
-N, --neuralnetwork-type <type> <numLayers> <numNLayers, ...> will generate a random neural network of the type specified
                                                types:
                                                        0. CPPN
                                                        1. FeedForward
                                                        2. Elman Network
-c, --color                             will use color as input for the neural network (can be used with -C)
-C, --coords                            will use coordinates as input for the neural network (can be used with -c)
-bw                                     the output image is black and white.
-s, --save <filename>                   will save the neural network generated.
-sg, --save-genome <filename>           will save the genome generated.
-f, --file-output <filename>            image that will output.
-res, --resolution <width height>       will create a image of that resolution(ignored if --image is specified).
--image <filename>                      load a image and generate another.
-r, --repeat <n>                        It will generate the image again.(use it for recurrent nn)
-h, --help                              help menu (This)
```