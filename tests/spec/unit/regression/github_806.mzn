/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

type foo = tuple(int);
test bar(array [$T] of foo: x) = true;
constraint bar([(1,)]);
