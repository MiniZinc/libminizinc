/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

% GitHub #997
% Reified maximum_arg could not be used in reified context if a solver had only the non-reified as a builtin. 

include "alldifferent.mzn";
include "arg_max.mzn";

array[1..10] of 0..1 : test_bool = [ 0, 0, 1, 0, 0, 0, 1, 1, 1, 1];
array[1..10] of var 0..1 : test_bool_2;

array[1..10] of var 1..20 : test_int;
array[1..10] of var 0..20 : test_int_2;

constraint (sum(i in 1..10) (test_bool_2[i])) = 6;

constraint alldifferent([ test_int[i] | i in 1..10 where test_bool[i] == 1]);

constraint forall(i in 1..10) (
(test_bool_2[i] == 1) -> maximum_arg([test_int[j] | j in 1..i], test_int_2[i])
);
