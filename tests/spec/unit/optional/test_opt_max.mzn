/***
!Test
expected: !Result
  solution: !Solution
    y: 3
***/

include "globals.mzn";

array [1..5] of var opt 1..3: x;
var 1..10: y;

constraint occurs(x[1]);
constraint forall (i in 1..4) (occurs(x[i]) = absent(x[i + 1]));
constraint y = max(x);
constraint all_different(x);

solve satisfy;
