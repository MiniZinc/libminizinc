/***
!Test
expected:
- !Result
  solution: !Solution
    x: 1
***/

% Used to give UNSATISFIABLE

var {1}: x;

constraint int_times(1, 1, x);

solve satisfy;
