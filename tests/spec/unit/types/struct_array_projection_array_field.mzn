/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: |-
      [[1, 2], [4, 5, 6]]|true|6
      [[1, 2]]|[1, 2]|[3, 7]
      [[10, 20], [30, 40]]|40|true|true
      3|[[3, 4]]|[3, 4]|[1..2, 4..5]
***/

% Regression test for projecting an array-valued field out of an array of structs.
%
% The result is an array of arrays, which MiniZinc represents by wrapping the inner array in a
% marker tuple (a tuple whose second field is unknown), exactly as vComprehension does for an
% array-valued comprehension body. The projection did not build that wrapper: it let
% Type::arrType drop the field's own dimensions and produce an array of the *element* type, so
% the model was accepted with a wrong type and then failed in evaluation with
% "not an integer expression" -- or, for a var field, handed malformed FlatZinc to the solver.
%
% The projection must agree with the hand-written comprehension it is desugared into,
% `[x[i].f | i in index_set(x)]', so that equivalence is asserted directly.

any: x = [([1, 2], 3), ([4, 5, 6], 7)];
any: proj = x.1;
any: hand = [x[i].1 | i in index_set(x)];

% Record spelling.
any: r = [(f: [1, 2], g: 3)];

% Ragged inner arrays, and the index sets of both the outer and the inner array, must survive.
enum E = {A, B};
enum F = {P, Q};
array[E] of any: e = [A: (array1d(F, [10, 20]), 1), B: (array1d(F, [30, 40]), 2)];

% A two-dimensional inner array keeps its dimensionality.
any: m = [(array2d(1..2, 1..2, [1, 2, 3, 4]), 9)];

% A var array field: this is the case that used to reach the solver as malformed FlatZinc.
array[1..2] of var 1..9: a;
any: v = [(a, 1)];
constraint v.1[1][1] = 3 /\ v.1[1][2] = 4;

% Sets are not arrays, so a set-valued field is projected as an ordinary element type.
any: s = [({1, 2}, 3), ({4, 5}, 6)];

solve satisfy;

output [
  show(proj), "|", show(proj == hand), "|", show(proj[2][3]), "\n",
  show(r.f), "|", show(x.1[1]), "|", show(x.2), "\n",
  show(e.1), "|", show(e.1[B][Q]), "|", show(index_set(e.1) == E), "|",
  show(index_set(e.1[A]) == F), "\n",
  show(m.1[1][2, 1]), "|", show(v.1), "|", show(a), "|", show(s.1),
];
