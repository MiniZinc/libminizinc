% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS ON minizinc_cpx

include "strict_lex2.mzn";

array[1..2, 1..2] of var 1..2: y;

constraint strict_lex2([|2, 2, 3|
                         2, 3, 1|]);

constraint strict_lex2(y);

solve satisfy;

output ["y = array2d(1..2, 1..2, ", show(y), ");\n"];
