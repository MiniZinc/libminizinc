/***
!Test
expected:
- !Result
  solution: !Solution
    P: -2147483645
    Q: -2147483644
    R: -2147483646
- !Result
  solution: !Solution
    P: 1
    Q: 2
    R: 0
- !Result
  solution: !Solution
    P: -499999999
    Q: -499999998
    R: -500000000
***/

% Regression test for bug #282: mzn2fzn's optimization pass left dangling
% variables in the int_search/4 annotation below.

var int : P;
var int : Q;
var int : R;
constraint ((R + 1) = P);
constraint ((R + 2) = Q);
solve :: seq_search([int_search([P,Q,R], input_order, indomain_min, complete)])
                satisfy;
output [
	"P = ", show(P), "\n",
	"Q = ", show(Q), "\n",
	"R = ", show(R), "\n"
];
