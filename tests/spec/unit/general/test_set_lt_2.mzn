/***
--- !Test
solvers: [gecode, chuffed] # Too slow on CBC
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution:
  - !Solution
    sets:
    - !!set {}
    - !!set {1}
    - !Range 1..2
    - !Range 1..3
    - !!set {1, 3}
    - !!set {2}
    - !Range 2..3
    - !!set {3}
--- !Test
solvers: [cbc]
expected: !Result
  status: SATISFIED
  solution: !Solution
    sets:
    - !!set {}
    - !!set {1}
    - !Range 1..2
    - !Range 1..3
    - !!set {1, 3}
    - !!set {2}
    - !Range 2..3
    - !!set {3}
***/

% Lexicographic order of subsets of 1..3

int: n = 3;
int: m = pow(2, n);

array[1..m] of var set of 1..n: sets;

constraint forall (i in 1..m-1) (sets[i] < sets[i+1]);

output [show(sets)];

solve satisfy;