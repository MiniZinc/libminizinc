/***
--- !Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    a: 16
    b: 1
  - !Solution
    a: 16
    b: 2
  - !Solution
    a: 16
    b: 4
  - !Solution
    a: 16
    b: 8
  - !Solution
    a: 16
    b: 16
***/

int: a :: add_to_output = product([|2, 2 | 2, 2|]);

array[1..2, 1..2] of var 1..2: vs;
var int: b :: add_to_output = product(vs);

solve satisfy;
