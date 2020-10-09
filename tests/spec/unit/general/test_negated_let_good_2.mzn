/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: [0]
  - !Solution
    x: [2]
***/

array[1..1] of var 0..2: x;
constraint not (let { var int: y = x[x[1]] } in y > 0);

solve :: int_search(x, input_order, indomain_min, complete)
      satisfy;
