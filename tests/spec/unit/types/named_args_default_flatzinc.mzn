/***
!Test
solvers: [gecode]
type: compile
expected: !FlatZinc named_args_default_flatzinc.fzn
***/

% Defaults are expanded into the call during typechecking, so the omitted
% argument is visible in the generated FlatZinc (`foo(a,5)`). Making the
% defaulted value explicit in the output is intentional.

predicate foo(var int: x, var int: y = 5);
var 0..10: a;
constraint foo(a);

solve satisfy;
