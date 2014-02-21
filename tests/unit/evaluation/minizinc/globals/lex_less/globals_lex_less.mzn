% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON mzn_mer_lp
% RUNS ON mzn_mer_lazyfd
% RUNS ON minizinc_cpx

include "lex_less.mzn";

%-----------------------------------------------------------------------------%
% lex_less
%-----------------------------------------------------------------------------%
array[1..5] of var -100..100: lex_avi1 = [1,3,5,7,9];
array[4..8] of var -100..100: lex_avi2 = array1d(4..8, [1,3,5,_,9]);

constraint lex_less(lex_avi1, lex_avi2);

solve satisfy;

output [
   "lex_avi1 = array1d(1..5, ", show(lex_avi1), ");\n",
   "lex_avi2 = array1d(4..8, ", show(lex_avi2), ");\n"
];
