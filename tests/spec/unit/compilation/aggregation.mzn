/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc aggregation.fzn
***/

% This tests is dependend on the successful aggregation of sum(xs) into the
% int_lin_le expression and the removal of intermediate value for sum(xs) that
% is created

array[1..2] of var 1..4: xs;

predicate my_lin(array [int] of int: as,array [int] of var int: bs,int: c);
predicate int_lin_eq(array [int] of int: as,array [int] of var int: bs,int: c)
  = my_lin(as, bs, c);

constraint sum(xs) <= 5;
