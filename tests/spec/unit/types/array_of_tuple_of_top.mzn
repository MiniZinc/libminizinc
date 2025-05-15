/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

test foo($T: x) = let {
  any: y = [(x,), (x,)];
} in true;
