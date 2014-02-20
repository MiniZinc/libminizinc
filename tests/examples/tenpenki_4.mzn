% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON mzn_mer_lp
% RUNS ON zinc_fdic_mznlib
% RUNS ON zinc_mip_hl
% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx
include "tenpenki.mzn.model";

nrows = 3;
ncols = 3;

constraint row_constraint(1, [3]);
constraint row_constraint(2, [2]);
constraint row_constraint(3, [1]);

constraint col_constraint(1, [3]);
constraint col_constraint(2, [2]);
constraint col_constraint(3, [1]);

% Solution:
% 
%   # # #
%   # # .
%   # . .

