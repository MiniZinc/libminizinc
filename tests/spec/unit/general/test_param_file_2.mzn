/***
---
!Test
solvers: [gecode]
options:
  param-file: './spec/unit/general/test_param_file_2.json'
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: true
    y: false
***/

bool: x :: add_to_output;
bool: y :: add_to_output;
