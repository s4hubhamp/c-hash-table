## Run

`cc -Wall -Wextra -pedantic -ggdb -o executable main.c utils.c && ./executable t8.shakespeare.txt`

## Benchmarks

Ran multiple times on **Apple M1 Pro**
The has table performs ~ 450 times faster than linear search.

```
size of t8.shakespeare.txt is 5458199 bytes
Analyzing t8.shakespeare.txt with Linear search
Size: 5458199 bytes
  Tokens: 67506
  Top 10 tokens: 
    0: the 23242
    1: I 19540
    2: and 18297
    3: to 15623
    4: of 15544
    5: a 12532
    6: my 10824
    7: in 9576
    8: you 9081
    9: is 7851
Elapsed time 31.198s
Analyzing t8.shakespeare.txt with Hash Table
Size: 5458199 bytes
  Tokens: 67506 tokens
  Top 10 tokens
    0: the 23242
    1: I 19540
    2: and 18297
    3: to 15623
    4: of 15544
    5: a 12532
    6: my 10824
    7: in 9576
    8: you 9081
    9: is 7851
Elapsed time 0.062s
```
