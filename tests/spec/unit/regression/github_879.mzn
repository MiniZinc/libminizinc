/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

enum Foo = {A};
any: a :: output = [enum2int(A): 1 | i in 1..1];
any: b :: output = index2int([A: 1])[A];
any: c :: output = [enum2int(A) | i in 1..1];
any: d :: output = [enum2int(A): enum2int(A) | i in 1..1];
