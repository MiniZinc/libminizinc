/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: [2, 3]
***/

% The counterpart to named_args_builtin_rejected: a positional call to the same
% low-level builtin resolves exactly as it always has. Builtins are
% positional-only in this release, not unavailable.

array [1..2] of var 0..10: a;
constraint int_lin_eq([1, 1], a, 5);
constraint a[1] = 2;

solve satisfy;
