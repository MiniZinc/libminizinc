/***
!Test
solvers: [gecode]
expected:
- !Result
  solution: !Solution
    v: null
- !Result
  solution: !Solution
    v: 1
***/

array[1..2] of record(1..2: x): records = [(x: 1), (x: 2)];

var opt 1..1: v;

constraint if occurs(v) then records[deopt(v)].x = 1 endif;

solve satisfy;
