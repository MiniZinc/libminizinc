/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

function array [$$E, $$F, $$G, $$H, $$I, $$J] of int: foo(
  array [int] of $$E: a,
  array [int] of $$F: b,
  array [int] of $$G: c,
  array [int] of $$H: d,
  array [int] of $$I: e,
  array [int] of $$J: f
) = [(i, j, k, l, m, n): 1 | i in a, j in b, k in c, l in d, m in e, n in f];

enum Foo = {A};
any: x :: output = foo([A], [A], [A], [A], [A], [A]);
