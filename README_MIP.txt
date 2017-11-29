MIP Solver Interfaces in MiniZinc 2
===================================

The executables mzn-cplex, mzn-gurobi, mzn-scip and mzn-cbc use the
corresponding MIP solver library. They can interpret FlatZinc code compiled with
-Glinear, as well as handle original model files (by flattening + solving).

All MIP solvers directly support multi-threading. For this, CBC needs to be
configured with --enable-cbc-parallel. Use svn/git to get the latest stable CBC revision,
see https://projects.coin-or.org/Cbc, currently https://projects.coin-or.org/svn/Cbc/stable/2.9

Calling a solver on a MiniZinc directly:
  
      mzn-gurobi -v -s -a -G linear model.mzn data.dzn

or separated flattening+solving - sometimes more stable but slower due to file I/O:
 
      mzn2fzn -G linear model.mzn data.dzn && mzn-gorubi -v -s -a model.fzn | solns2out model.ozn

INDICATOR CONSTRAINTS
===================================
Some solvers (CPLEX, Gurobi) have indicator constrains.
Add command-line parameters -D fIndConstr=true -D fMIPdomains=false when flattening

USER CUTS and LAZY CONSTRAINTS
===================================
Apply annotations ::MIP_cut and/or ::MIP_lazy after a constraint.
For Gurobi, see share/minizinc/linear/options.mzn for their exact meaning.
   PLEASE NOTE:
   If you export FZN file with lazy_constraint/user_cut annotations,
   their declarations are not exported currently (as of 7.11.17).
   WORKAROUND: when solving that fzn, add -G linear,
   e.g., as follows: mzn-cplex -G linear model.fzn

WARM STARTS
===================================
Annotations warm_start and warm_start_array for the solve item can provide 'hint' values
for the solver. Example:

array[1..3] of var 0..10: x;
array[1..3] of var 0.0..10.5: xf;
var bool: b;
array[1..3] of var set of 5..9: xs;
solve
  :: warm_start( [b], [true] )
  :: warm_start_array( [
       warm_start( x, [2,8,4] ),
       warm_start( xf, [5.6,5.8,4.7] ),
       warm_start( xs, [ {}, 6..8, {3,6} ] )
     ] )
  minimize x[1] + b + xf[2] + card( xs[1] intersect xs[3] );

If you'd like to provide a most complete warmstart information, please provide values for all
variables which are output when there is no solve item or when compiled with --output-mode dzn.
Still, this excludes auxiliary variables introduced by let's. To capture them, you can customize
the output item, or try the FlatZinc level, see below.

USING WARM STARTS AT THE FLATZINC LEVEL
You can insert warm start information in the FlatZinc in the same way for all non-fixed variables.
Just make sure the fzn interpreter outputs their values by annotating them as output_var(_array)
and capture the fzn output by, e.g., piping to solns2out --output-raw <file_raw.dzn>.
You can also insert high-level output into FZN warm start. When compiling the initial model, add
empty warm start annotations for all important variables - they will be kept in FZN. In the next solve,
fill the values. To fix the order of annotations, put them into a warm_start_array.

LIMITATIONS
1. Currently implemented for CPLEX and Gurobi.
2. Currently not automated, e.g., adding a solution in dzn format as a warm start during parsing
(future work).


