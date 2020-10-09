/***
!Test
expected:
- !Result
  solution: !Solution
    a:
    - [true, false, true, true, true]
    - [false, true, true, false, false]
    - [true, true, true, true, true]
    - [false, false, true, true, false]
    - [true, true, true, false, true]
***/

include "tenpenki.mzn.model";

nrows = 5;
ncols = 5;

constraint row_constraint(1, [1, 3]);
constraint row_constraint(2, [2]);
constraint row_constraint(3, [5]);
constraint row_constraint(4, [2]);
constraint row_constraint(5, [3, 1]);

constraint col_constraint(1, [1, 1, 1]);
constraint col_constraint(2, [2, 1]);
constraint col_constraint(3, [5]);
constraint col_constraint(4, [1, 2]);
constraint col_constraint(5, [1, 1, 1]);

% Solution:
%
%   # . # # #
%   . # # . .
%   # # # # #
%   . . # # .
%   # # # . #

