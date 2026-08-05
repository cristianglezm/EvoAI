# EvoAI Documentation

## Building

To make the documentation you will need the following programs:

* [Doxygen](https://www.doxygen.nl)
* [graphviz](https://graphviz.org/)
* [Sphinx](https://www.sphinx-doc.org/en/master/index.html)

use the following command to install Sphinx and the rest of its dependencies.

```bash
pip install -r requirements.txt
cd ../build
cmake -DEvoAI_BUILD_DOCS=TRUE ..
cmake --build . -j 4 -t docs
```
