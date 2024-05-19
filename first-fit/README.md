# First Fit

Iterate the memory and allocate a process in the first free space that it fits the process.

```
process A        free space        process B         free space
+---+---+---+    +---+---+----+    +---+----+---+    +---+----+------+
| A | 0 | 4 | -> | - | 4 | 10 | -> | B | 14 | 6 | -> | - | 20 | 1004 |
+---+---+---+    +---+---+----+    +---+----+---+    +---+----+------+
                        |
                        +---> Chosen if the process
                              has up to 10 space
```
