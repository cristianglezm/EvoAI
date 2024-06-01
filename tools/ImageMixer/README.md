# Image Mixer

* This tool will mix images and alter it by the weights of the neural network.

## Example

* This will create a CPPN genome load input1 and input2 and use the neural network made from the genome to mix the images.

```bash
ImageMixer -G 0 2 --image 2 input1.png input2.png -bw -f output.png
```

## Tool help
```bash
ImageMixer <options>
-g, --genome [m|r] <filename> [<filename> with r]       load a genome json file
                                        With m will mutate the genome.
                                        With r will combine two genomes, without m or r will load the genome.
-G, --genome-type <type> <numHidden>    will generate a genome of the type specified
                                                types:
                                                        0. Without hidden neurons
                                                        1. With hidden neurons.
-bw                                     the output image is black and white.
-s, --save <filename>                   will save the neural network generated.
-sg, --save-genome <filename>           will save the genome generated.
-f, --file-output <filename>            image that will output.
--image <n> <filename>...                       load n images and mix them.
-h, --help                              help menu (This)
```