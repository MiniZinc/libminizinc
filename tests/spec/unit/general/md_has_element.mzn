/***
--- !Test
options:
  all_solutions: true
solvers: [gecode]
expected: !Result
  solution: !SolutionSet
  - !Solution
    a: true
    b: false
    c: true
    d: false
    x: [[[1, 1], [1, 1]], [[1, 1], [1, 3]]]
***/

bool: a :: add_to_output = has_element(3, [|1, 2 | 3, 4|]);
bool: b :: add_to_output = has_element(5, [|1, 2 | 3, 4|]);
bool: c :: add_to_output = has_element(3, [|1, <> | 3, <>|]);
bool: d :: add_to_output = has_element(2, [|1, <> | 3, <>|]);

array[1..2, 1..2, 1..2] of var 1..3: x :: add_to_output;
constraint has_element(3, x);
constraint not has_element(2, x);
constraint forall (i, j, k in 1..2 where i + j + k < 6) (x[i, j, k] = 1);
