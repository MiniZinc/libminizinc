/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a:
    - 0.5
    - 1.0
    - 1.5
    - 2.0
***/

int: n = 4;

% Per-element float domain returned by a function call (no literal in the
% domain expression).  The old syntactic guesser defaulted to int here and
% miscompiled; proper type inference must pick float from the function's
% declared return type.
function set of float: dom_of(int: c) = { int2float(c) / 2.0 };

array[c in 1..n] of var dom_of(c): a ::output;
constraint forall(c in 1..n)(a[c] = int2float(c) / 2.0);

solve satisfy;
