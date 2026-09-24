/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    r: ["optset", "optset", "poly", "set", "poly"]
***/

% Concrete overloads take priority over polymorphic ones
function string: f(var opt $T: x) = "poly";
function string: f(var opt set of int: x) = "optset";
function string: h(var opt $T: x) = "poly";
function string: h(var set of int: x) = "set";
var opt set of 1..2: a;
var set of 1..2: s;
array [int] of string: r :: output = [f(a), f(s), h(a), h(s), f(1)];
