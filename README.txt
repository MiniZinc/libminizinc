MiniSearch 0.1
==============

This package contains the source code and binary files for the MiniSearch 
meta-search language, which includes the MiniZinc 2.1beta constraint 
modelling language and tool chain. It provides a backend that supports all
FlatZinc solvers.


** Getting Started **

The binary files are in the bin/ directory and you find examples in the 
example/ directory. The MiniSearch binary is called minisearch. You can 
run an example with your FlatZinc solver by typing, for instance

./bin/minisearch --solver <your-fzn-solver> examples/golomb_lns.mzn

into your shell in this directory. Checkout the documentation on how to
get a FlatZinc solver if have not got any installed on your system.


** Documentation **

For a detailed documentation please visit:
http://www.minizinc.org/minisearch


** Bugs **

If you encounter any problems with MiniSearch, please use the MiniZinc 
bug tracker at http://www.minizinc.org/trac to report any issues or 
feature requests.
