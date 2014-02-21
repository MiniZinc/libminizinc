% RUNS ON mzn_mer_fd

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
