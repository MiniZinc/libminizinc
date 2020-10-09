/***
--- !Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution:
  - !Solution
    a: 55.0
--- !Test # Workaround for chuffed all-solution issue
solvers: [chuffed]
expected: !Result
  solution: !Solution
    a: 55.0
***/

float : a :: add_to_output = sum([|1.0, 2.0, 3.0, 4.0, 5.0 | 6.0, 7.0, 8.0, 9.0, 10.0|]);

solve satisfy;
