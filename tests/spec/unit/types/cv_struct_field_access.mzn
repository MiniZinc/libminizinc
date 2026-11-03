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

% my_index_set(c) is par but contains variables (cv). Its body accesses a field of a tuple-valued
% call with the var argument c. The tuple was evaluated directly, evaluating the definition of
% the variables in c (assertion failure in eval_par).
function tuple(set of $$E): my_index_sets(array [$$E] of any $U: x) = (index_set(x),);
function set of int: my_index_set(array [int] of var bool: x) = my_index_sets(x).1;
var 1..3: x;
constraint let { array [int] of var bool: c = [x > 1, true] } in card(my_index_set(c)) > 0;
