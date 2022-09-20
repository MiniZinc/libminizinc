/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [[A, A], [A, A]]
***/
enum Foo = {A, B};
array [1..2, 1..2] of var Foo: x;
constraint forall (i in 1..2) (count (j in 1..2) (x[i, j] = A) = 2);
