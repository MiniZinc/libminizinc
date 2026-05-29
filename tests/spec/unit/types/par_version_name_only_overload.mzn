/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: 105
    b: 205
    p: 1
    q: 2
***/

% Regression test: "name-only" overloads (declarations that differ only in a
% parameter name and are dispatched via named arguments) each need their own
% auto-generated par version. Previously the par-version pass keyed functions
% by their type-only match, so two name-only siblings collapsed onto a single
% par version: a par-argument call to one sibling was rewritten to the other
% sibling's (par) body, silently running the wrong code. It also made the
% sibling-name detector spuriously warn (no name-compatible rewrite target).

% Par functions: the returned value reveals which body ran.
function int: bounded(int: upper) = 100 + upper;
function int: bounded(int: lower) = 200 + lower;

% Predicates with par-evaluable bodies called with par arguments: the call is
% narrowed to its par version during flattening and must reach its own body.
predicate pick_p(var int: upper) = p = 1;
predicate pick_p(var int: lower) = p = 2;
predicate pick_q(var int: upper) = q = 1;
predicate pick_q(var int: lower) = q = 2;

var int: a :: output = bounded(upper: 5);
var int: b :: output = bounded(lower: 5);
var 1..2: p :: output;
var 1..2: q :: output;

constraint pick_p(upper: 0);
constraint pick_q(lower: 0);

solve satisfy;
