/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

opt int: i = <>;
any: x = [(1,)][i];
