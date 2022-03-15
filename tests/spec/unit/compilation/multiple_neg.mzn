/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    A: 0
    B: 0
    C: 0
  - !Solution
    A: 0
    B: 0
    C: 1
  - !Solution
    A: 1
    B: 0
    C: 0
  - !Solution
    A: 1
    B: 0
    C: 1
  - !Solution
    A: 1
    B: 1
    C: 0
  - !Solution
    A: 1
    B: 1
    C: 1
***/
var 0..1: A;
var 0..1: B;
var 0..1: C;
constraint not((((B=0) -> (B=1)) -> (A=1))) -> not(((C=1) -> true));
