/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    BV0: false
    IV0: 0
***/

var bool: BV0;
var 0..10: IV0;
constraint (BV0) == (IV0);
constraint (sum([1*(BV0),-1*(IV0)])) == 0;
constraint not BV0;
constraint (IV0) == 0;
solve satisfy;
