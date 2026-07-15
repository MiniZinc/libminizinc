/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: "call to `cumulatives' is ambiguous"
type: compile
***/

% The two 5-argument overloads of cumulatives differ only in the name of their
% last parameter (upper_bounds vs lower_bounds), so a purely positional 5-argument
% call cannot pick one and must be a type error. A named argument is required to
% choose the intended bound direction.

include "cumulatives.mzn";

array [1..2] of var 0..10: s;
array [1..2] of var 0..10: d;
array [1..2] of var 0..10: r;
array [1..2] of var 1..2: m;
array [1..2] of int: b = [5, 5];

constraint cumulatives(s, d, r, m, b);

solve satisfy;
