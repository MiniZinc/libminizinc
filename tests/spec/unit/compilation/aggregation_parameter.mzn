/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc aggregation_parameter.fzn
***/

% Expect one direct int_lin_le with no intermediate sum because predicate
% parameter binding is transparent and leaves only one reader.

array [1..4] of var 0..10: a;

predicate p(var int: x) = x <= 5;

constraint p(sum(a));
