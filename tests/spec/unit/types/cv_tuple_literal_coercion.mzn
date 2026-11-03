/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 1
  - !Solution
    x: 2
  - !Solution
    x: 3
***/

% The tuple literal (my_index_set(c),) contains variables (cv), because my_index_set(c) has a var
% argument. Coercing it to the declared type tuple(set of int) dropped the cv flag, so it was
% evaluated directly, evaluating the definition of the variables in c (assertion failure in
% eval_par, or crash).
function tuple(set of $$E): my_index_sets(array [$$E] of any $U: x) = (index_set(x),);
function set of int: my_index_set(array [int] of var bool: x) = my_index_sets(x).1;
var 1..3: x;
constraint let {
  array [int] of var bool: c = [let { var bool: b = x > 1 } in b, true];
  tuple(set of int): t = (my_index_set(c),);
} in card(t.1) > 0;
