/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    p: 10
    q: 5
***/

% A solver library may rename a builtin's parameters. Because the builtin family
% is anchored (body-less) and therefore positional-only, its overloads are
% re-matched by type alone during flattening, so a positional call still
% resolves exactly as before regardless of the renaming. Here the body-less
% anchor `my_prim(x, y)` fixes the canonical names and the bodied "solver"
% override `my_prim(a, b)` renames them; the positional call reaches the
% override's body (p = 2*q = 10). A named call to `my_prim` would instead be
% rejected as a builtin.

predicate my_prim(var int: x, var int: y);
predicate my_prim(var int: a, var int: b) = a = 2 * b;

var 0..100: p;
var 0..100: q;
constraint my_prim(p, q);
constraint q = 5;

solve satisfy;
