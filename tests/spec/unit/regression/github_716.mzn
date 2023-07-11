/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    BV9: false
    BV8: true
    BV7: false
    BV4: true
  - !Solution
    BV9: true
    BV8: true
    BV7: false
    BV4: true
  - !Solution
    BV9: false
    BV8: false
    BV7: true
    BV4: true
  - !Solution
    BV9: true
    BV8: false
    BV7: true
    BV4: true
***/
var bool: BV9;
var bool: BV8;
var bool: BV7;
var bool: BV4;

constraint (BV9) \/ ((not BV8) == (BV7));
constraint ((not BV8) == (BV7)) == (BV4);
constraint BV4;
