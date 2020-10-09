/***
!Test
expected:
- !Result
  solution: !Solution
    A3: false
    B3: true
- !Result
  solution: !Solution
    A3: true
    B3: false
***/

% Regression test for bug #283: mzn2fzn's optimization pass left dangling
% variables in the bool_search/4 annotation below.

var bool : A3;
var bool : B3;
constraint (A3 <-> (not B3));
solve :: seq_search([
                bool_search([A3], input_order, indomain_min, complete)])
                satisfy;
output [
   "A3 = ", show(A3), "\n",
   "B3 = ", show(B3), "\n"
];
