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
    x: [[null, true], [true, null]]
***/

bool: a :: add_to_output = forall([|true, <> | <>, true|]);
bool: b :: add_to_output = forall([|true, <> | false, <>|]);
bool: c :: add_to_output = forall([<>, <>]);

array[1..2, 1..2] of var opt bool: x :: add_to_output;
constraint forall(x);

constraint absent(x[1, 1]) /\ occurs(x[1, 2]) /\ occurs(x[2, 1]) /\ absent(x[2, 2]);
