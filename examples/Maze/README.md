# Maze

This example generates a maze using the **GrowingTree** algorithm and animates 
letters travelling from the centre to targets at the bottom row, spelling out the 
input text. 

Each letter is assigned a random pathfinding algorithm and processed concurrently 
on a thread-pool.

```bash
Usage: 

Maze [text] [--seed N] [--randomness F] [--binomial F] [-w N] [-h N]
    [text]                               Text to spell out in the maze. (default: "Hello, World")
    --seed N                             RNG seed for reproducible runs. (default: current clock time)
    --randomness F                       GrowingTree bias towards the newest cell [0.0–1.0]. (default: random)
    --binomial F                         GrowingTree binomial selection weight [0.0–1.0]. (default: random)
    -w N                                 Maze width in columns. (default: max(25, len(text) + 6))
    -h N                                 Maze height in rows. (default: 25)
    --help                               Print this help message and exit.
```

Each letter is coloured by the algorithm it used to find its path:

| Colour  | Algorithm |
|---------|-----------|
| Cyan    | DFS       |
| Green   | BFS       |
| Yellow  | GBFS      |
| Magenta | A\*       |

After the animation completes, per-letter and per-algorithm pathfinding metrics 
(time in ns, path length) are printed to stdout.
