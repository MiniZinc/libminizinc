/***
!Test
expected:
- !Result
  solution: !Solution
    xs: [1, null, 2]
***/

include "strictly_increasing.mzn";

array[1..3] of var opt 1..2: xs;

constraint occurs(xs[1]) /\ occurs(xs[3]);
constraint strictly_increasing(xs);
