/***
!Test
solvers: [gecode]
expected:
- !Result
  solution: !Solution
    x: [null]
    b: true
- !Result
  solution: !Solution
    x: [1]
    b: false
***/

% Test to ensure that if-then-else correctly handles equality with absent

array [1..1] of var opt 1..1: x;
var bool: b;

constraint if b then x[1] = <> else x[1] = 1 endif;
