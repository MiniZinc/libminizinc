/***
!Test
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    a: false
    b: false
    c: true
    d: true
  - !Solution
    a: false
    b: false
    c: false
    d: true
  - !Solution
    a: false
    b: false
    c: true
    d: false
  - !Solution
    a: false
    b: false
    c: false
    d: false
  - !Solution
    a: true
    b: false
    c: false
    d: false
  - !Solution
    a: false
    b: true
    c: false
    d: false
  - !Solution
    a: true
    b: true
    c: false
    d: false
options:
  all_solutions: true
solvers: [gecode]
***/
var bool: a;
var bool: b;
var bool: c;
var bool: d;

constraint not(forall([a \/ b] ++ [c \/ d]));
