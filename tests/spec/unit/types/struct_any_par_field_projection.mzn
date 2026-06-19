/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    c: 3
    w: 5
***/

% Regression test for a compiler abort in eval_par.
% A polymorphic `any $T` function instantiated at a record that mixes a par
% field (`s`) and a var field (`v`) used to lose the par-ness of the par field.
% Projecting the par field in a par context (`card`) then aborted with
%   "Assertion failed: (l->type().isPar()), function eval_par".
% The var field (`v`) is projected as well, to pin down that the par projection
% still forces eval_par of the record while the var projection keeps flattening
% normally.

function any $T: id(any $T: x) = x;

record(set of int: s, var int: v): r = (s: {1, 2, 3}, v: let { var 4..6: vi } in vi);

constraint card(id(r).s) = 3;   % par projection of the par field (par context)
constraint id(r).v = 5;         % var projection of the var field

solve satisfy;

any: c :: output = card(id(r).s);
any: w :: output = id(r).v;
