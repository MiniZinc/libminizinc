/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

test foo(tuple(array [int] of int): v) =
  let {
    tuple(array [int] of int): x = v;
  } in true;
any: a = foo(([1, 2],));
any: b = foo(([1, 2, 3],));
