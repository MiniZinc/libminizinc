/***
!Test
solvers: [gecode]
extra_files:
- checker_var_bug.mzc.mzn
expected:
- !Result
  solution: !Solution
    _checker: "x = 1;\n"
- !Result
  solution: !Solution
    _checker: "x = 2;\n"
***/

solve satisfy;
