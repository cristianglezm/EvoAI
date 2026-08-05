# EvoAI - General Overview and Agent Context

This repository contains **EvoAI**, a modern C++17 library designed for evolving and/or training small Neural Networks. 

1. Ask, don't assume. If something is unclear, ask before writing a single line. Never make silent assumptions about intent, architecture, or requirements. When running unattended, pick the most reasonable interpretation, proceed, and record the assumption rather than blocking.
2. Implement the simplest solution for simple problems, better solutions for harder problems. Do not over-engineer or add flexibility that isn't needed yet. 
3. Don't touch unrelated code but please do surface bad code or design smells you discover with me so we can address them as a separate issue.
4. Flag uncertainty explicitly. If you're unsure about something, see point 1 above. If it makes sense to do so, conduct a small, localised and low-risk experiment and bring the hypothesis and results to me to discuss. Confidence without certainty causes more damage than admitting a gap.
5. I'm always open to ideas on better ways to do things. Please don't hesitate to suggest a better way, or one that has long lasting impact over a tactical change. (as a few examples)
6. When writing comments, write why, not what, if the code is complex it's ok to explain what it does too, but most code doesn't need a comment at all. Don't overshare context I already gave you by repeating it in the comment (e.g. "this is for project X to do Y and Z"). Example: given context that a function computes a checksum for a network protocol -- don't write "// computes the checksum for project X's protocol" (that just restates what I told you); do write "// XOR-fold, not CRC: protocol only needs error *detection*, and this runs on every packet" (that explains a non-obvious why). Default to no comment at all unless there's a real why worth capturing.

When generating code or answering questions about this repository, use the following overview as context:

## Core Features and Architecture
- **Neuroevolution Algorithms**: EvoAI supports evolving Neural Networks using the NEAT (NeuroEvolution of Augmenting Topologies) and HyperNEAT algorithms. It uses a `Genome` architecture to manage topological mutations and recombinations.
- **Population & Speciation**: Evolution is handled via the `EvoAI::Population` template class, which includes speciation logic. Any class that fulfills the `EvoAI::meta::is_populable_v<T>` trait constraint can be evolved.
- **Classical Deep Learning**: In addition to evolutionary approaches, EvoAI provides a complete suite for traditional supervised learning. It features DataLoaders, standard Layers/Neurons, Backpropagation support, various Loss functions (MSE, CrossEntropy), Optimizers (SGD, Adam, Muon), and Learning Rate Schedulers.
- **Generic Algorithms**: Implements generic path-finding, maze-generation, and genetic selection tools (such as Tournament selection) within the library logic.
- **Compile-time interfaces**: We use them around the codebase for max customization for the user, as we use c++17 we use is_detected idiom instead of concepts.

## Directory Structure
- **`include/EvoAI/`**: Public headers defining the library's API.
- **`src/`**: Implementation files for the core functionalities.
- **`examples/`**: Contains demonstration apps, such as `CellSim`, `IrisClassification`, `Maze`, `NumClassifier`, and `XOR`.
- **`tools/`**: Utilities utilizing the library: `GenomeVisualizer`, `ImageEvolver`, `ImageGenerator`, `ImageMixer`, `NeuralNetworkVisualizer`, and `SoundGenerator`. Includes visual applications that use SFML.
- **`test/`**: High-coverage unit tests built with GoogleTest (`googletest`).
- **`docs/`**: Sphinx-based documentation generation with CMake configs.

## Development Guidelines
- **C++ Standard**: Code must be strictly C++17 compliant (or dictated by library specs).
- **Dependencies**: EvoAI uses `JsonBox` for serialization. Tools and visualizers optionally use `SFML`.
- **Namespacing**: Ensure new core logic is placed correctly within the `EvoAI` namespace. Follow the separation between definitions in `.hpp`, inline implementations in `.inl`, and generic source definitions in `.cpp` files.
- **Build System**: The project relies exclusively on CMake for compilation and target definitions, with native support for Linux, Windows, macOS, Android, and Emscripten (WebAssembly).
- **Exhaustiveness via warnings**: Enum switch statements over things like activation types intentionally omit a case for sentinel/count values (e.g. `LAST_CPPN_ACTIVATION_TYPE`), so `-Wswitch` fires if a new enumerator is added and a switch site is forgotten to update. Don't "fix" these warnings by adding a catch-all `default:` -- that silently defeats the check.
