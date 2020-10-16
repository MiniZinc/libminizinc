/***
---
!Test
solvers: [gecode]
options:
  param-file: './spec/unit/param_file/param_file_array.json'
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: true
    y: false
***/

bool: x :: add_to_output;
bool: y :: add_to_output;
