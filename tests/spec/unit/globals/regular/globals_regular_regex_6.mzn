/***
!Test
expected: !Result
  solution: !Solution
    x: [A, B, A, C, A, D, A]
***/

include "regular_regexp.mzn";

int: n = 7;
enum Letter = {A, B, C, D};
array [1..n] of var Letter: x :: add_to_output;

constraint regular(x, "A ((B|C|D) A)*");

constraint forall (i in 1..n-2 where i mod 2 = 0) (x[i] < x[i+2]);