/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    y: 5
***/

enum Foo = {A, B};
array [Foo] of int: x = [5, 10];
function $T: foo(array [$$E] of $T: a) = a[min(index_set(a))];
any: y :: output = foo(x);
