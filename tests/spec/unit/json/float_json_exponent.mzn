/***
!Test
extra_files:
- float_json_exponent.json
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: 2.0e-05
    y: 1500.0
***/
float: x ::output;
float: y ::output;
