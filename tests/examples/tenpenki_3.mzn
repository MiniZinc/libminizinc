% RUNS ON mzn20_fd
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

