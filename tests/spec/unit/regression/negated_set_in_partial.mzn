/***
!Test
solvers: [gecode]
expected:
- !Result
  status: UNSATISFIABLE
***/

% Regression test: negating a set membership (or subset) whose set operand is a
% partial expression (here a var-index array access that may be out of bounds)
% used to be flattened with a double negation. The reified value was produced in
% a still-negated context and then negated again by conj(), so `not (1 in HO[h])`
% collapsed to `defined(HO[h]) -> (1 in HO[h])` and the constraint was dropped,
% making the model spuriously satisfiable.
%
% Here card(HO[1]) = 1 forces HO[1] = {1}; h != 0 with the array access forces
% h = 1, so 1 in HO[h] holds and `b = not (1 in HO[h])` must be false, which
% contradicts `constraint b`. The model is therefore UNSATISFIABLE.
array [1..1] of var set of 1..1: HO;
constraint card(HO[1]) = 1;
var 0..1: h;
var bool: b = not (1 in HO[h]);
constraint h != 0;
constraint b;
solve satisfy;
