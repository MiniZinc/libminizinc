/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    f: [2, true]
    z: ["a", 2]
***/

% The fields of a struct keep their own inst when passed to a `var $T' parameter, so a
% var record with a par string field is a valid instantiation
record(string: n, var 1..3: x): r = (n: "a", x: 2);
tuple(int, bool) : f :: output = (fix(r).x, is_fixed(r));
function var $T: g(var $T: x) = x;
any: z :: output = g(("a", r.x));
