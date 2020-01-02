/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: Ok
***/

include "alldifferent_except_0.mzn";

constraint alldifferent_except_0([]);
constraint alldifferent_except_0([0, 0]);
constraint alldifferent_except_0([1, 2, 3]);
constraint alldifferent_except_0([-2, -1, 0, 1, 2]);

solve satisfy;

output ["Ok"];
