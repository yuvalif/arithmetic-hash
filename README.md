## arithmetic-hash
order preserving, imperfect, arithmetic hash.
code is based on the code provided together with this [blog post](https://marknelson.us/posts/2014/10/19/data-compression-with-arithmetic-coding.html).
however, the goal is different. there is no intent to compress and decompress data. instead, the idea is to create a one directional hash of large strings that would be:
* imperfect: looking for high colission rate (bins should contain many strings)
* order preserving: S1 > S2 iff hash(S1) > hash(S2)
* balanced: similar number of strings in each bin
* hashing is done line-by-line

## build
```
g++ -Wall --std=c++20 -o hash hash.cpp
g++ -Wall --std=c++20 -o generate_names generate_names.cpp
```

# run
to create a file with lines to hash:
```
./generate_names <# prefixes> <# of name in perfix> > names.txt
```
to hash the lines (into memory):
```
./hash names.txt
```
