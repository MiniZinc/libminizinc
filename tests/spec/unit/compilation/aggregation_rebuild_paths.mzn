/***
!Test
solvers: [gecode]
options:
  keep-paths: true
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    p: [4, 0]
    q: [0, 3]
  - !Solution
    p: [4, 1]
    q: [0, 3]
  - !Solution
    p: [4, 2]
    q: [0, 3]
  - !Solution
    p: [4, 3]
    q: [0, 3]
  - !Solution
    p: [4, 4]
    q: [0, 3]
  - !Solution
    p: [4, 0]
    q: [0, 4]
  - !Solution
    p: [4, 1]
    q: [0, 4]
  - !Solution
    p: [4, 2]
    q: [0, 4]
  - !Solution
    p: [4, 3]
    q: [0, 4]
  - !Solution
    p: [4, 4]
    q: [0, 4]
***/

% The relations lb[i] <= ub[i] are built at the end of flattening, outside the
% call stack of the forall. With paths kept, the sums of both relations must not
% be merged into one variable because they come from the same source location.

array [1..2] of var 0..4: p;
array [1..2] of var 0..4: q;
array [1..2] of var bool: b;
array [1..2] of var int: lb = [p[i] div 2 + b[i] - q[i] | i in 1..2];
array [1..2] of var int: ub = [q[i] div 3 + p[i] - b[i] | i in 1..2];
constraint forall (i in 1..2) (lb[i] <= ub[i]);
constraint p[1] = 4 /\ q[1] = 0 /\ b[1];
constraint q[2] >= 3 /\ not b[2];
