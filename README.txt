MiniZinc 2
==========

This package contains the MiniZinc 2 constraint modelling
language and tool chain. Compared to the previous version 1.6,
it is a complete re-write of the MiniZinc-to-FlatZinc
compiler, based on the new libminizinc C++ library.

For installation and compilation instructions, as well as a small
start-up guide, please refer to the file INSTALL.txt.

Changes from version 1.6
------------------------

MiniZinc 2.0 contains many new features and is based on a complete rewrite of
the MiniZinc-to-FlatZinc compiler. If you are currently using the previous
version 1.6, the new tools can be used as drop-in replacements. The generated
FlatZinc is compatible with version 1.6, so all FlatZinc solvers should work
without changes.

** MiniZinc language changes **

 - MiniZinc now supports user-defined functions. Details have been published
   in the paper "MiniZinc with Functions". Both functions and predicates can
   be recursive.
 - MiniZinc now supports option types. Details have been published in the
   paper "Modelling with Option Types in MiniZinc".
 - Let expressions have been generalised. They can now contain constraint
   items in addition to variable declarations.
 - Array index sets can be declared using arbitrary set expressions as long as
   they evaluate to contiguous ranges.
 - The if-then-else expression has been generalised to allow the condition to
   be a var bool expression (instead of only par bool).
 - Array and set comprehensions as well as generator calls can now iterate
   over variables and use var bool where conditions.
 - Any bool expression can now automatically coerce to an int expression,
   likewise for int and float. This means that you don't have to write
   bool2int or int2float in you models any more.
 - Equality constraints can now be posted between array expressions.
 - Arbitrary expressions can now be included ("interpolated") into strings,
   using the syntax "some text \(e) some more text", where e is any
   expression. It is the same as writing "some text "++show(e)++" some more
   text".

** New built-in functions **

Array functions: array1d, arrayXd, row, col, has_index, has_element, sort_by,
sort, arg_sort, arg_min, arg_max

** New global constraints **

 - arg_max, arg_min
 - arg_sort
 - k-dimensional diffn
 - disjunctive
 - geost
 - knapsack
 - network_flow
 - regular with NFAs
 - symmetric all different
 - optional scheduling constraints: alternative, span, disjunctive, cumulative
 - functional versions of many global constraints

** New tool chain **

 - There are a few new builtins that solvers can reimplement, these are listed
   in the redefinitions-2.0 file.
 - Include items use a different method for finding included files. Paths are
   now interpreted as relative to the file that has the include item. That
   way, the mzn2fzn compiler can be called from a different working directory.
 - A new tool, mzn2doc, can produce html output from the documentation
   comments. The MiniZinc distribution contains the documentation for global
   constraints and builtins generated directly from the library source code.
 - Executable mzn-fzn is a replacement for the minizinc driver: option --solver
   can define a different flatzinc interpreter (default: 'flatzinc')
   
** New MIP solver interfaces **

 - The executables mzn-cplex, mzn-gurobi, mzn-scip and mzn-cbc use the
   corresponding MIP solver. They can interpret FlatZinc code compiled with
   -Glinear, as well as handle original model files (by flattening + solving).
   Some MIP-specific functionality has been implemented, such as warm starts.
   See README_MIP.txt.

** Bugs **

If you encounter any problems with MiniZinc, please use the MiniZinc bug tracker
at https://github.com/MiniZinc/libminizinc/issues to report any issues or feature requests.
