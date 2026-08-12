/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: |-
      (1, 2)|3|[(1, 2), (4, 5)]
      4|[3, 6]|[(1, 2)]
      (4, 5)|(3, 4)
      20|[10, 20]|[3, 6]
***/

% Regression test for indexing or slicing the result of a projection.
%
% A field access on an array is a projection, which `add_coercion' rewrites into a comprehension.
% Nothing else performs that rewrite, so a projection that no enclosing context coerces survives
% into evaluation as a field access whose object is an array, which `eval_fieldaccess' cannot
% handle:
%   "Assertion failed: (Expression::type(fa->v()).istuple() || Expression::type(fa->v()).isrecord()),
%    function eval_fieldaccess".
% Array access was the one such context: `vArrayAccess' re-coerced its object only when that
% object was an `ArrayAccess' or a `Call', so `x.1[1]' aborted while `sum(x.1)', `x.1 ++ x.1',
% `[v | v in x.1]' and plain `x.1' were all fine. This is independent of the field type, so a
% scalar field is covered here as well as a struct-valued one.

any: x = [((1, 2), 3), ((4, 5), 6)];
any: r = [(f: (1, 2), g: 3), (f: (4, 5), g: 6)];
enum E = {A, B};
array[E] of any: e = [A: ((1, 2), 5), B: ((3, 4), 6)];

% A projection whose object is a *scalar* struct holding an array is not a projection at all and
% has always worked; it is pinned down here because it goes through the same code path now.
any: t = ([10, 20, 30], 9);

array[1..2] of var 1..9: v;
constraint x.2[1] = 3;              % projection indexed in a constraint, not just in output
constraint v[1] = x.2[1] /\ v[2] = x.2[2];

solve satisfy;

output [
  show(x.1[1]), "|", show(x.2[1]), "|", show(x.1[1..2]), "\n",
  show(x.1.1[2]), "|", show([x.2[i] | i in 1..2]), "|", show([x.1[1]]), "\n",
  show(r.f[2]), "|", show(e.1[B]), "\n",
  show(t.1[2]), "|", show(t.1[1..2]), "|", show(v),
];
