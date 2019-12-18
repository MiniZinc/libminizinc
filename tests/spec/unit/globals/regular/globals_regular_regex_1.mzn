/***
!Test
expected: !Result
  solution: !Solution
    x: [1, 2, 2, 4]
***/

include "regular_regexp.mzn";

array [1..4] of var 1..4: x :: add_to_output;

constraint regular(x, "1 2* 4");