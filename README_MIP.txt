MIP Solver Interfaces in MiniZinc 2
===================================

The executables mzn-cplex, -gurobi, -scip, -xpress and -cbc were replaced by the new
executable minizinc with the option --solver. Please run it with --help <solverId>
to see available functionalities.

All MIP solvers directly support multi-threading (option -p). For this, CBC needs to be
configured with --enable-cbc-parallel. Use svn/git to get the latest stable CBC revision,
see https://projects.coin-or.org/Cbc, currently https://projects.coin-or.org/svn/Cbc/stable/2.9

Calling a solver on a MiniZinc model directly:
  
      minizinc --solver -v -s -a model.mzn data.dzn

or separated flattening+solving - sometimes more stable but slower due to file I/O:
 
      minizinc --solver osicbc -c model.mzn data.dzn && minizinc --solver osicbc -v -s -a model.fzn | minizinc --ozn-file model.ozn

USEFUL FLATTENING PARAMETERS
============================
-D nSECcuts=0/1/2                %% Subtour Elimination Constraints, see below
-D fMIPdomains=true/false        %% See below
-D float_EPS=1e-6                %% Epsilon for floats' strict comparison
-D fIndConstr=true/false         %% Use solvers' indicator constraints, see below

SUBTOUR ELIMINATION CONSTRAINTS
===============================
Optionally use the SEC cuts for the circuit global constraint.
Currently only Gurobi and CPLEX. (2018/09)
If compiling from source, this needs boost and #define COMPILE_BOOST_MINCUT
in lib/algorithms/min_cut.cpp (cmake determines automatically).
Values of nSECcuts: 0,1: use MTZ formulation; 1,2: pass on circuit constraints
to the SEC cut generator, so 1 would use both (seems best)

UNIFIED DOMAIN REFINEMENT (MIPdomains)
===================================
The 'MIPdomains' feature of the Flattener aims at reducing the number of binary flags
encoding linearized domain constraints, see

   [1] Belov, Stuckey, Tack, Wallace. Improved Linearization of Constraint Programming Models. CP 2016.

By default it is on, but for some models such as packing problems, it is better off.
To turn it off, add option -D fMIPdomains=false during flattening.
Some parameters of the unification are available, run with --help.

INDICATOR CONSTRAINTS
===================================
Some solvers (CPLEX, Gurobi) have indicator constrains with greater numerical stability.
Add command-line parameters -D fIndConstr=true -D fMIPdomains=false when flattening
to use them when (half-)reifying.

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
constraint b+sum(x)==1;
constraint b+sum(xf)==2.4;
constraint 5==sum( [ card(xs[i]) | i in index_set(xs) ] );
solve
  :: warm_start( [b], [true] )
  :: warm_start_array( [
       warm_start( x, [<>,8,4] ),               %%% Use <> for missing values
       warm_start( xf, array1d(-5..-3, [5.6,<>,4.7] ) ),
       warm_start( xs, array1d( -3..-2, [ 6..8, 5..7 ] ) )
     ] )
  :: seq_search( [ int_search(x, first_fail, indomain_min, complete)  ] )
  minimize x[1] + b + xf[2] + card( xs[1] intersect xs[3] );

If you'd like to provide a most complete warmstart information, please provide values for all
variables which are output when there is no output item or when compiled with --output-mode dzn.
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


