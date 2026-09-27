/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    cmp: [false, false, true, true, false, false, true, false, true, true, true, true, true]
    au: !!set {1.5, 2.5, 3.5}
    ai: !!set {2.5}
***/

% Comparisons of par float sets used to crash (they were evaluated as int sets), and
% array_union/array_intersect had no builtin for float sets
set of float: b = {1.5, 2.5};
set of float: c = {2.5, 3.5};
array [int] of bool: cmp :: output = [
  b subset c, c superset b, b < c, b <= c, b > c, b = c, b != c, {} = b, {} subset b, b = b,
  {1.5} subset b, 1.5..2.5 superset b, b >= b,
];
set of float: au :: output = array_union([b, c]);
set of float: ai :: output = array_intersect([1.0..3.0, b, c]);
