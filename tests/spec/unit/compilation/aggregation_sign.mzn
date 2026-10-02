/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc aggregation_sign.fzn
***/

% Expect one shared integer disequality and one shared float equality because
% equality and disequality are canonicalised up to sign.

var 0..10: x;
var 0..10: y;
var 0.0..10.0: u;
var 0.0..10.0: v;
var bool: p;
var bool: q;
var bool: r;
var bool: s;

constraint (x - y != 0) \/ p;
constraint (y - x != 0) \/ q;
constraint (u - v = 0.0) \/ r;
constraint (v - u = 0.0) \/ s;
