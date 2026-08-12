# Fuzzing EvoAI's JSON deserialization

libFuzzer harnesses for the constructors that turn untrusted JSON into
EvoAI types: anything that saves/loads a `NeuralNetwork`, `Genome`, or
`Graph`/`LatticeGraph` goes through one of these, whether that's EvoAI's
own file format or a downstream consumer passing
user-supplied JSON.

## Building

Clang only (libFuzzer + ASan/UBSan aren't available with GCC here), native
only (no WASM):

```sh
cmake -S . -B build/fuzz -DCMAKE_CXX_COMPILER=clang++ -DEvoAI_BUILD_FUZZERS=ON
cmake --build build/fuzz -j4
```

Or, for local convenience, the `linux-fuzz` CMake preset does the same
thing (see `CMakePresets.json`):

```sh
cmake --workflow --preset linux-fuzz
```

Binaries land in `build/fuzz/fuzz/bin/`.

## Running

```sh
build/fuzz/fuzz/bin/fuzz_neuralnetwork_from_json -max_total_time=60 test/fuzz/corpus
build/fuzz/fuzz/bin/fuzz_genome_from_json        -max_total_time=60 test/fuzz/corpus
build/fuzz/fuzz/bin/fuzz_graph_from_json         -max_total_time=60 test/fuzz/corpus
build/fuzz/fuzz/bin/fuzz_jsonbox_parse           -max_total_time=60 test/fuzz/corpus
```

Each harness parses the fuzzer's bytes as JSON, constructs the target type,
and exercises it the way a real consumer would (not just construction -
`run()`/`backward()` for `NeuralNetwork`, `makePhenotype()` for `Genome`,
walking every node's outgoing edges for `Graph`), since several of the
bugs found so far only manifested once the deserialized object was
*used*, not at construction time.

## What's covered / not covered

- `fuzz_jsonbox_parse` - JsonBox's own parser, with nothing EvoAI-specific
  on top. Every other harness here assumes JsonBox either parses
  successfully or throws a catchable exception on malformed input; this
  one exists specifically to test that assumption in isolation.
- `fuzz_neuralnetwork_from_json` - `NeuralNetwork(JsonBox::Object)`.
- `fuzz_genome_from_json` - `Genome(JsonBox::Object)` +
  `Genome::makePhenotype()`.
- `fuzz_graph_from_json` - `Graph<>(JsonBox::Object)` (the default
  `NodeDefault`/`EdgeDefault` instantiation, used by `LatticeGraph`).
- Not covered yet: `HyperNeat`/`SubstrateInfo` (no index-trust risk found
  there - it's plain data, and its `stoull` sites already got the
  non-throwing-parse fix everywhere else did), `Population`/`Species`
  (JSON parsing hardened the same way as everything else, but not
  independently fuzzed yet).

## Findings so far (all fixed, kept here for context)

- **`NeuralNetwork`: out-of-range connection indices.** A connection's
  `src`/`dest` layer/neuron indices came straight from JSON with nothing
  validating them against the actual layer/neuron counts -
  `layers[dest.layer][dest.neuron]` in `run()` (and several other
  methods) would go out of bounds. Fixed by validating every connection
  right after `layers` is built, throwing `std::out_of_range` on the
  first invalid one.
- **`NeuralNetwork`: empty `layers`.** `{"layers":[]}` is structurally
  valid JSON that the fix above didn't catch (nothing to iterate), but
  `run()`/`backward()` do `layers.back()` and `setInputs()` does
  `layers[0]` unconditionally. Fixed by rejecting empty `layers` in the
  same validation pass.
- **`Genome::makePhenotype()`: empty genome.** A genome with no node
  chromosomes (e.g. the default constructor, or one deserialized from
  JSON with an empty/missing `nodeChromosomes` array) produces three
  empty phenotype layers, and `makePhenotype()` unconditionally read
  `nn[2][0]` (the first output neuron) to check its activation type.
  Fixed by guarding that whole step behind an emptiness check -
  `makePhenotype()` is `noexcept` and used in evolutionary hot loops, so
  it degrades gracefully instead of throwing.
- **`Graph`: out-of-range edge indices.** `addDirectedEdge()` does
  `head[e.prev]` with no bounds check, and `e.prev`/`e.next` came straight
  from JSON. Unlike the `NeuralNetwork` case, this didn't crash at
  construction - `et.next` is only stored as data on the edge, not used
  to index anything until later - so it silently built a structurally
  corrupt graph that only crashed the first time something walked its
  edges (e.g. pathfinding). Fixed by validating both indices where edges
  are parsed, before wiring them into the forward-star.
- **`Graph`: untrusted node index.** Found by actually running this
  harness, not a manual audit - `forEachOutgoingEdge()` does
  `head[n.index]` with no bounds check, and `NodeDefault::index` came
  straight from JSON with nothing reconciling it against the node's real
  position in `nodes`. A node whose JSON `"index"` didn't match its
  actual position let a caller OOB `head[]` even through a node obtained
  via a valid, in-bounds `operator[]`. Reproduced with
  clang+libFuzzer+ASan (`heap-buffer-overflow ... in
  forEachOutgoingEdge`), found in under 2400 runs from the seed corpus
  alone. Fixed by resetting every deserialized node's `.index` to match
  its actual array position, the same invariant `addNode()` already
  enforces at runtime - not just validating, since the field is entirely
  derivable from position and shouldn't be trusted from JSON at all.

