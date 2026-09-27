/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    r1: !!set {1.5}
    r2: !!set {1.5, 2.5}
    r3: !!set {1.5}
    x: 2
  status: SATISFIED
***/

% User-defined operators with a polymorphic body must be specialised like calls, otherwise par
% evaluation of the body fails with $T unknown (internal error "invalid set literal type")
function set of $T: 'union'(opt set of $T: x, set of $T: y) = (x default {}) union y;
function set of $T: '-'(set of $T: x) = x;
opt set of float: p = <>;
opt set of float: p2 = {2.5};
set of float: q = {1.5};
any: r1 :: output = p union q;
any: r2 :: output = p2 union q;
any: r3 :: output = -q;
var 1..2: x;
constraint x > card(p2 union q) - 1;
output ["\(p2 union q) \(x)\n"];
