# GameOfLife
Non-visual implementation of Game of Life.

* Reads in a file representing a world.
* First line must be the size of the world (e.g 1000,250 -> 1000 columns, 250 rows).
* 'x' -> Alive
* '.' -> Dead



Commands:

|  |  |
| --- | --- |
| `--load <filename>` | Load specified Game of Life World |
| `--save <filename>` | Save calculated world after n-generations to file |
| `--generations <generation count>` | Defines the number of generations used |
| `--measure` | Whether the needed time should be printed |
