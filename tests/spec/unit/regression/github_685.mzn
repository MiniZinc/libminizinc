/***
!Test
solvers: [gecode]
expected: 
  - !Result
    solution: !Solution
      x: [false, true]
      y: [true, false]
  - !Result
    solution: !Solution
      x: [true, false]
      y: [false, true]
***/
include "lex_lesseq.mzn";

int: N = 2;

array [1..N] of var bool: x;
array [1..N] of var bool: y;

constraint lex_less(x, y) \/ lex_less(y, x);
constraint count(x) = 1;
constraint count(y) = 1;
