/***
!Test
solvers: [gecode, cp-sat]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: null
    o: false
  - !Solution
    x: !!set {}
    o: true
  - !Solution
    x: !!set {1}
    o: true
  - !Solution
    x: !!set {2}
    o: true
  - !Solution
    x: !!set {1, 2}
    o: true
  status: ALL_SOLUTIONS
***/

% An absent set has a unique representation, so it is only enumerated once
var opt set of 1..2: x;
var bool: o :: output = occurs(x);
