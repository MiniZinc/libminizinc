MIP Solver Interfaces in MiniZinc 2
===================================

The executables mzn-cplex, mzn-gurobi, mzn-scip and mzn-cbc use the
corresponding MIP solver library. They can interpret FlatZinc code compiled with
-Glinear, as well as handle original model files (by flattening + solving).

All MIP solvers directly support multi-threading. For this, CBC needs to be
configured with --enable-cbc-parallel. Use svn/git to get the latest CBC revision,
see https://projects.coin-or.org/Cbc.

Calling a solver on a MiniZinc directly:
  
      mzn-gurobi -v -s -a -G linear model.mzn data.dzn

or separated flattening+solving - sometimes more stable but slower due to file I/O:
 
      mzn2fzn -G linear model.mzn data.dzn; mzn-gorubi -v -s -a model.fzn | solns2out model.ozn

USER CUTS and LAZY CONSTRAINTS
===================================
Apply annotations ::MIP_cut and/or ::MIP_lazy after a constraint.
For Gurobi, see share/minizinc/linear/options.mzn for their exact meaning.

