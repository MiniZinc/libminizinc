MiniZinc 2.0 beta release
=========================

This is a beta release of the upcoming MiniZinc 2.0 constraint modelling
language and tool chain. It is a complete re-write of the MiniZinc-to-FlatZinc
compiler, based on the new libminizinc C++ library.

For installation and compilation instructions please refer to the file
INSTALL.txt.

Changes from version 1.6
------------------------

Both the MiniZinc language and the mzn2fzn compiler have changed significantly.

MiniZinc:
  - now supports user-defined functions. Details have been published in the 
    paper "MiniZinc with Functions" which can be found in the doc/pdf folder. 
    Functions can be recursive.
  - now supports option types. Details have been published in the paper
    "Modelling with Option Types in MiniZinc" which can be found in the doc/pdf
    folder.
  - has generalised let expressions, which can now contain constraint items in 
    addition to variable declarations.
  - can have arbitrary set expressions as array index sets (however they have 
    to evaluate to contiguous ranges)
  - has a generalised if-then-else expression where the condition can be a
    var bool expression (instead of only par bool).
  - uses a different method for finding included files. Paths are now interpreted
    as relative to the file that has the include item. That way, the mzn2fzn
    compiler can be called from a different working directory.

mzn2fzn:
  - is now based on the libminizinc C++ library. The current beta version ships
    as a drop-in replacement for the previous mzn2fzn compiler. However, the
    underlying library provides a programmatic API to all the functionality.
    Through the API, you can construct models, add data, and even invoke
    solvers directly. The API will be finalised and documented in a future
    release.

FlatZinc:

The mzn2fzn compiler produces FlatZinc that is backwards-compatible with
MiniZinc 1.6. This means that all FlatZinc interpreters should work with the
new version of MiniZinc. However, there are some changes and simplifications:

 - Variable arrays are now merely containers and are always defined. This means 
   that when FlatZinc 1.6 would contain an item like this:
   array[1..3] of var 1..3: x;
   The new FlatZinc will look like this:
   var 1..3: x_1;
   var 1..3: x_2;
   var 1..3: x_3;
   array[1..3] of var 1..3: x = [x_1,x_2,x_3];
   As a consequence, FlatZinc does not contain any array accesses (like x[3]) 
   any more, which should make it easier to implement solver interfaces.
 - Some builtins have been deprecated.

Current limitations
-------------------

This is a beta release, which is not feature complete, has certain restrictions 
and contains bugs. Here is a list of known issues:
  - The lb, ub and fix functions may sometimes fail with an error message that 
    the bounds of the expression could not be determined. In this case you may 
    need to decompose the expression manually, i.e. introduce a variable for 
    the expression and use lb/ub on that variable.
  - Anonymous variables are only supported on the right hand side of array 
    variable declarations and assign items. Furthermore, at least one element 
    of the array must be non-anonymous. This is a limitation in the type 
    checker that we will address in a future update.

If you find additional bugs, please report them using the MiniZinc bug tracker:
http://www.minizinc.org/trac

