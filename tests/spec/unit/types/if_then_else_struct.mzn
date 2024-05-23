/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    t: [null, null]
    b: false
    u: [0, 0]
***/

any: t :: output = if true then (<>, <>) else (false, false) endif;
bool: b :: output = occurs(t.1);
any: u :: output = if false then (1, 1) else (false, false) endif;
