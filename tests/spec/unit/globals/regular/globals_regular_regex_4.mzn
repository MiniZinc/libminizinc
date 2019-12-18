/***
!Test
expected:
- !Result
  solution: !Solution
    x: [1, 1, 1, 2, 2, 4, 4, 4, 4, 4]
- !Result
  solution: !Solution
    x: [1, 1, 1, 1, 2, 2, 4, 4, 4, 4]
- !Result
  solution: !Solution
    x: [1, 1, 1, 1, 1, 2, 2, 4, 4, 4]
***/

include "regular_regexp.mzn";

array [1..10] of var 1..4: x :: add_to_output;

constraint regular(x, "1{3,5} 2{2} 4+");