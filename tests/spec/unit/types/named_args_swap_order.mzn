/***
!Test
solvers: [gecode]
type: compile
expected: !FlatZinc named_args_swap_order.fzn
***/

% A named-argument call must bind by name, not by position. The step-5
% silent-mis-resolution finding was that `foo(y: 99, x: 11)` compiled to
% positional `foo(99, 11)`; step 6's resolver reorders into the matched
% decl's parameter order so the FlatZinc reads `foo(11, 99)`.

predicate foo(int: x, int: y);
constraint foo(y: 99, x: 11);

solve satisfy;
