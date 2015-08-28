MiniSearch 0.1
==============

This package contains the binary files for the MiniSearch meta-search 
language, which includes the MiniZinc 2.1beta constraint modelling 
language and tool chain. It provides a backend that supports all
FlatZinc solvers.

The source code can be obtained from the feature/minisearch branch on 
github: https://github.com/MiniZinc/libminizinc/tree/feature/minisearch
by calling:

git clone -b feature/minisearch https://github.com/MiniZinc/libminizinc.git



** Getting Started **

The minisearch binary file is in the bin/ directory and you can find 
examples in the example/ directory. You can run an example with your 
FlatZinc solver by typing, for instance for Linux or Mac OS:

./bin/minisearch --solver <your-fzn-solver> examples/golomb_lns.mzn

or Windows:

cd bin
minisearch.exe --solver <your-fzn-solver.exe> ../examples/golomb_lns.mzn

into your shell in this directory. Check out the documentation on how 
to get a FlatZinc solver if have not got any installed on your system.

Please note that you will need to set the MiniZinc standard library 
environement variable $MZN_STDLIB_PATH to share/minizinc to include 
the MiniSearch builtins. You can also manually set the stdlib path by 
adding the --stdlib-dir option to calling minisearch for Linux/Mac OS:

./bin/minisearch --solver <your-fzn-solver> --stdlib-dir share/minizinc/ examples/golomb_lns.mzn

or, for Windows:

cd bin
minisearch.exe --solver <your-fzn-solver.exe> --stdlib-dir ../share/minizinc ../examples/golomb_lns.mzn



** Documentation **

For a detailed documentation please visit:
http://www.minizinc.org/minisearch



** Bugs **

If you encounter any problems with MiniSearch, please use the MiniZinc 
bug tracker at http://www.minizinc.org/trac to report any issues or 
feature requests.
