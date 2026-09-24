/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    s: a
    f: !!set {1.5}
    v: 2
***/

% `any $T' can be instantiated with types that have no var version (strings, sets of float)
function any $T: g(any $T: x) = x;
any: s :: output = g("a");
any: f :: output = g({1.5});
var 2..2: w;
any: v :: output = g(w);
