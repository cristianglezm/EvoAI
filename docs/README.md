# EvoAI Documentation

* TODO
    + personalize theme.
        - reduce section nav width

## Building

To make the documentation you will need the following programs:

* [Doxygen](https://www.doxygen.nl)
* [graphviz](https://graphviz.org/)
* [Sphinx](https://www.sphinx-doc.org/en/master/index.html)

use the following command to install the Sphinx and the rest of its dependencies.

```bash
python install -r requirements.txt
cd build
make -j4 docs
```
