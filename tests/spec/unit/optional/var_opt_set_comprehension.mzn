/***
!Test
solvers: [gecode, cp-sat, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: [true, false, false]
    c1: [!!set {1}, null, null]
    c2: [!!set {1}, null, null]
    c3: [null, null, !!set {}]
  - !Solution
    b: [true, true, false]
    c1: [!!set {1}, null, null]
    c2: [!!set {1}, null, null]
    c3: [null, null, !!set {}]
  status: ALL_SOLUTIONS
***/

% Comprehensions producing arrays of var opt sets: variable where clauses (with var opt set and
% par opt set bodies) and variable set generators
array [1..3] of var opt set of 1..2: a :: no_output;
constraint a = [{1}, <>, {}];
array [1..3] of var bool: b;
var set of 1..3: s :: no_output;
constraint card(s) = 1;
array [int] of var opt set of int: c1 :: output = [a[i] | i in 1..3 where b[i]];
array [int] of var opt set of int: c2 :: output =
  [if i = 2 then <> else {i} endif | i in 1..3 where b[i]];
array [int] of var opt set of int: c3 :: output = [a[k] | k in s];
constraint b[1] /\ not b[3] /\ 3 in s;
