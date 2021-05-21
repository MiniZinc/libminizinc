MiniZinc Flattening Benchmarks
==============================

## Setup

```sh
# Only required to run compare.py
pip install -r requirements.txt
```

## Running benchmarks

`flatten.py` creates a CSV file `statistics.csv` containing statistics output of flattening of a set
of instances in the MiniZinc Challenge directory structure (e.g. from the
[minizinc-benchmarks](https://github.com/minizinc/minizinc-benchmarks) repository). It will run
benchmarks in parallel by default. You can set the number of workers with `--parallel=<n>` where `n`
is the number of workers. Use `--parallel=0` (the default) for as many workers as CPUs, or
`-parallel=-1` for as many workers as instances. 

```sh
flatten.py --args="-Glinear" instances/ statistics.csv
```

It can also flatten a set of instances specified in CSV format (files are relative to CSV location).

`instances.csv`:
```
model1.mzn,data1.dzn
model2.mzn,data2.dzn
...
```

```sh
flatten.py --args="-Gstd" instances.csv statistics.csv
```

Use `--minizinc="/path/to/minizinc_binary"` to use a different MiniZinc installation (the default is the one found in your `PATH`).  
The `--prefix-args` option can be used to add a prefix to the `minizinc` command (e.g. `--prefix-args="timeout 10s"` to add a 10 second flattening timeout).

## Comparing benchmark runs

`compare.py` creates an HTML file which highlights significant differences in flattening performance
or statistics. These are considered to be changes of 10% or more (times are rounded for this
purpose, but the values reported are the true differences).

```sh
compare.py statistics_baseline.csv statistics_new.csv output.html
```
