/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    r: 735
***/

% Mixed positional and named arguments. Positional first, then named.
% `bar(7, b: 3, c: 5)` must bind 7 to `a` and 3, 5 to `b`, `c`. `bar` is a
% user function (named arguments are not available for positional-only
% builtins); its body encodes the binding so the solution reveals it:
% a*100 + b*10 + c = 735.

function int: bar(int: a, int: b, int: c) = a * 100 + b * 10 + c;
var 0..1000: r;
constraint r = bar(7, b: 3, c: 5);

solve satisfy;
