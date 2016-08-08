MIP Solver Interfaces in MiniZinc 2
===================================

The executables mzn-cplex, mzn-gurobi, mzn-scip and mzn-cbc use the
corresponding MIP solver. They can interpret FlatZinc code compiled with
-Glinear, as well as handle original model files (by flattening + solving).

All MIP solvers except OSI CBC directly support multi-threading. For CBC
compiled with enable-parallel, it is possible through --cbc-flags.
For models with non-integral objective function you might need to adjust
--absGap/--relGap/--objDiff.

Calling mzn-gurobi directly:
  
      mzn-gurobi -v -s -a -G linear model.mzn data.dzn

or, more stable but slower due to file I/O:
 
      mzn2fzn -G linear model.mzn data.dzn; mzn-gorubi -v -s -a model.fzn | solns2out model.ozn

