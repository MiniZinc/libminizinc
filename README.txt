MiniSearch 0.1
==============

This branch contains the source code for the MiniSearch meta-search 
language, which includes the MiniZinc 2.1beta constraint modelling 
language and tool chain. It provides a backend that supports all
FlatZinc solvers. 

With MiniSearch you can express and run many advanced meta-searches, 
such as lexicographic optimisation, large neighbourhood search, 
and/or search and diverse solution search. 

Have a look at the MiniSearch webpage for a detailed documentation:
http://www.minizinc.org/minisearch


** Folder Structure **

README.txt                   --->   this file
CMakeLists.txt               --->   cmake file
README_bin.txt               --->   README for binary package
lib/                         --->   cpp Files and libs
doc/                         --->   some MiniZinc documenation
solvers/                     --->   solver backends
tests/                       --->   test files and examples
LICENSE.txt                  --->   license file
md5_gen.cmake                --->   script for parser and lexer
share/                       --->   examples and MiniZinc libs
include/                     --->   header files



** Compilation **

To compile the sources, you require cmake (version 2.8.7 or newer),
flex, bison and a C++ compiler. Follow the instructions below for 
your OS in a terminal.

* Linux and Mac OS *
Create a build/ folder in the current directory and change into it
and execute cmake and make:

mkdir build
cd build
cmake ..
make

After compilation the build/ folder will contain the binary file
minisearch. See 'Getting Started' on how to execute it. 

* Windows *
Create a build/ folder in the current directory and change into it
and execute cmake:

mkdir build
cd build
cmake ..
cmake --build .

After compilation the build/Release folder will contain the binary file
minisearch.exe. See 'Getting Started' on how to execute it. 

* NOTE *

The instructions above will compile the sources for a 64bit machine. 
If you are compiling for a 32-bit architecture, then set the cmake 
option 'BUILD_32BIT' by:

cmake -DBUILD_32BIT=ON ..




** Building the binary package **

You can create the binary package using the following command
(after compilation) in the build/ directory:

cmake --build . --target package

This will create a .tar.gz (Linux and Mac OS) or .zip file (Windows)
in the build/ directory that contains the binary package.



** Getting Started **

* Linux and Mac OS *
After compilation, the minisearch binary file is in the build/ 
directory and you can find MiniSearch examples in the directory:
tests/minisearch/regression_tests/. You can run an example with 
your FlatZinc solver <fzn-solver> by typing, for instance

./build/minisearch --solver <fzn-solver> tests/minisearch/regression_tests/golomb_lns.mzn

into your shell in this directory. Check out the documentation on 
how to get a FlatZinc solver if have not got any installed on your 
system.

Note that you also have to set the MiniZinc standard library path
$MZN_STDLIB_DIR to share/minizinc/ since it contains the MiniSearch
builtin definitions. You can also manually set the stdlib path by 
adding the --stdlib-dir option to calling minisearch:

./build/minisearch --solver <fzn-solver> --stdlib-dir share/minizinc/ tests/minisearch/regression_tests/golomb_lns.mzn


* Windows *
After compilation, the minisearch binary file is in the 
build/Debug directory and you can find MiniSearch examples in 
the directory: tests/minisearch/regression_tests. You can run an 
example with your FlatZinc solver <fzn-solver.exe> by typing, for 
instance

cd build/Debug
minisearch.exe --solver <fzn-solver.exe> ../../tests/minisearch/regression_tests/golomb_lns.mzn

Note that you also have to set the MiniZinc standard library path
$MZN_STDLIB_DIR to share/minizinc since it contains the MiniSearch
builtin definitions. You can also manually set the stdlib path by 
adding the --stdlib-dir option to calling minisearch:

minisearch.exe --solver <fzn-solver.exe> --stdlib-dir ../../share/minizinc  ../../tests/minisearch/regression_tests/golomb_lns.mzn



** Documentation **

For a detailed documentation please visit:
http://www.minizinc.org/minisearch



** Bugs **

If you encounter any problems with MiniSearch, please use the MiniZinc 
bug tracker at http://www.minizinc.org/trac to report any issues or 
feature requests.
