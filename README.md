# Chaos Attractor Generator

This makes pretty background images. I wrote the original version around 2006. I wrote this version around 2008.

## Theory

TODO: explain iterated quadratic maps

TODO: explain search


## Usage

Generate a (x, y) sized jpeg, written to stdout.
```
./chaos x y
```

Generate a nicer looking jpeg, written to stdout (requires imagemagic)
```
./chaos x y | convert jpg: -normalize jpg:-
```

## TODO

* Higher dimensions
* More coefficients
* Dump coefficients
* Nicer colors