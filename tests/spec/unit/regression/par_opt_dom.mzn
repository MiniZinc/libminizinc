/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

predicate foo() = let {
  array [int] of var opt 1..2: x = [<>, 2];
} in true;

constraint foo();
