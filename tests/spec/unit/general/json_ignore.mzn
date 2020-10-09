/***
!Test
solvers: [gecode]
extra_files:
- json_ignore.mzc.mzn
- json_ignore.json
expected:
- !Result
  solution: !Solution
    _checker: "data_2 = 2;\n"
    data_1: 1
***/

int: data_1 :: add_to_output;
