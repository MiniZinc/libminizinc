/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 1
    y: null
    p: false
    a: 1
    b: null
    c: null
  - !Solution
    x: null
    y: 1
    p: false
    a: 1
    b: 1
    c: null
  - !Solution
    x: null
    y: 1
    p: true
    a: null
    b: null
    c: null
  - !Solution
    x: 1
    y: null
    p: true
    a: null
    b: 1
    c: null
  status: ALL_SOLUTIONS
***/

% Test various if-then-else cases with opt

var opt 1..1: x :: add_to_output;
var opt 1..1: y :: add_to_output;

var bool: p :: add_to_output;

var opt int: a :: add_to_output = if p then <> else 1 endif;
var opt int: b :: add_to_output = if p then x else y endif;

bool: q = true;
opt int: c :: add_to_output = if q then <> else 1 endif;

constraint not (x = y);