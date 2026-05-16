/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    total: 100
***/

int: n = 4;
function int: f(int: c) = c * 10;

% Indexed array declaration inside a let binder
var int: total ::output = let {
  array[c in 1..n] of var 1..f(c): x;
  constraint forall(c in 1..n)(x[c] = f(c));
} in sum(x);

solve satisfy;
