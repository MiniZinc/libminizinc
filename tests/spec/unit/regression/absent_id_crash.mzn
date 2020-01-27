/***
!Test
expected: !Result
  solution: !Solution
    z: [null, null]
***/

% Crashes due to assuming ids have a decl, when <> does not.

array [1..2] of var 0..1: x = [0, 0];
array [1..1] of var 1..1: y;

array [1..2] of var opt 1..1: z :: add_to_output = [y[x[i]] | i in 1..2 where x[i] != 0];
