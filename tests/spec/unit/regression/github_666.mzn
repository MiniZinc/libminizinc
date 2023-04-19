/***
!Test
solvers: [gecode]
options:
  -G: std
expected: !Result
  status: UNSATISFIABLE
***/

% Previously x = 1 was added to the CSE map in positive context
% and then retrieved in mixed context, while the decomposition was incorrectly
% left in positive context

predicate int_eq_imp(var int: a, var int: b, var bool: r);

var opt 1..1: x ::output;
constraint if x = <> then x = 1 endif;
constraint bool2int(x = 1) = 0;
