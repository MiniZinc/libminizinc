% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON minizinc_cpx

include "inverse_set.mzn";

%-----------------------------------------------------------------------------%
% inverse_set
%-----------------------------------------------------------------------------%
array[1..4] of var set of 1..5: invs_f1 =
	[{1}, _, {2,3,4}, _    ];
array[1..4] of var set of 1..5: invs_f2 =
	[_,   _, _,       {3,2}];

constraint inverse_set(invs_f1, invs_f2);

solve satisfy;

output [
   "invs_f1 = array1d(1..4, ", show(invs_f1), ");\n",
   "invs_f2 = array1d(1..4, ", show(invs_f2), ");\n"
];
