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
    x: [[null, null], [null, true]]
***/

bool: a :: add_to_output = exists([|false, <> | <>, true|]);
bool: b :: add_to_output = exists([|<>, false | <>, <>|]);
bool: c :: add_to_output = exists([<>, true]);

array[1..2, 1..2] of var opt bool: x :: add_to_output;
constraint exists(x);

constraint absent(x[1, 1]) /\ absent(x[1, 2]) /\ absent(x[2, 1]);
