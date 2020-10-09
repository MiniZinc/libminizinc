/***
!Test
solvers: [chuffed] # Too slow on cbc
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution:
  - !Solution
    sets:
    - !Range 1..3
    - !!set {1, 2, 4}
    - !!set {1, 2, 5}
    - !!set {1, 2, 6}
    - !!set {1, 3, 4}
    - !!set {1, 3, 5}
    - !!set {1, 3, 6}
    - !!set {1, 4, 5}
    - !!set {1, 4, 6}
    - !!set {1, 5, 6}
    - !Range 2..4
    - !!set {2, 3, 5}
    - !!set {2, 3, 6}
    - !!set {2, 4, 5}
    - !!set {2, 4, 6}
    - !!set {2, 5, 6}
    - !Range 3..5
    - !!set {3, 4, 6}
    - !!set {3, 5, 6}
    - !Range 4..6
***/

% Lexicographic order of 3-subsets of 1..6

set of int: elems = 1..6;
int: n = 3;
int: m = (product (i in card(elems)-n+1..card(elems)) (i)) div (product (i in 1..n) (i)); % choose n from elems

array [1..m] of var set of elems: sets;

constraint forall (i in 1..m) (card(sets[i]) = n);
constraint forall (i in 1..m-1) (sets[i] < sets[i+1]);

solve satisfy;

output [show(sets)];