% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON mzn_mer_lp
% RUNS ON zinc_fdic_mznlib
% RUNS ON zinc_mip_hl
% RUNS ON minizinc_cpx
% RUNS ON minizinc_fd
include "tenpenki.mzn.model";

nrows = 5;
ncols = 5;

constraint row_constraint(1, [2, 2]);
constraint row_constraint(2, [2, 2]);
constraint row_constraint(3, []);
constraint row_constraint(4, [1, 1]);
constraint row_constraint(5, [2]);

constraint col_constraint(1, [2]);
constraint col_constraint(2, [2, 1]);
constraint col_constraint(3, [1]);
constraint col_constraint(4, [2, 1]);
constraint col_constraint(5, [2, 1]);

% Solution:
%
%   # # . # #
%   # # . # #
%   . . . . .
%   . # . . #
%   . . # # .

