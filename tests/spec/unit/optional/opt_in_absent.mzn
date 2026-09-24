/***
!Test
solvers: [gecode, cp-sat]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: null
    s: !!set {2}
    p_in: true
    pf_in: true
    x_in: true
    x_in_s: true
  - !Solution
    x: 1
    s: !!set {2}
    p_in: true
    pf_in: true
    x_in: true
    x_in_s: false
  - !Solution
    x: 2
    s: !!set {2}
    p_in: true
    pf_in: true
    x_in: false
    x_in_s: true
  status: ALL_SOLUTIONS
***/

% <> in S is true (an absent element is ignored, like {<>} subset S)
opt int: p = <>;
opt float: pf = <>;
var opt 1..2: x;
var set of 1..2: s;
bool: p_in :: output = p in {1};
bool: pf_in :: output = pf in {1.0};
var bool: x_in :: output = x in {1};
var bool: x_in_s :: output = x in s;
constraint s = {2};
