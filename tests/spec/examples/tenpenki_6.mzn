/***
!Test
expected:
- !Result
  solution: !Solution
    a:
    - [false, false, false, false, false, false, true, true, false, false]
    - [false, false, false, false, false, false, true, false, true, false]
    - [true, false, false, false, false, false, true, true, true, true]
    - [true, false, false, false, false, true, true, true, false, false]
    - [true, false, false, false, true, true, true, true, false, false]
    - [true, true, true, true, true, true, true, true, false, false]
    - [true, true, true, true, true, true, true, false, false, false]
    - [false, true, true, true, true, true, true, false, false, false]
    - [false, false, true, true, true, true, false, false, false, false]
    - [false, false, false, true, true, false, false, false, false, false]
***/

include "tenpenki.mzn.model";

nrows = 10;
ncols = 10;

constraint row_constraint( 1, [2]);
constraint row_constraint( 2, [1, 1]);
constraint row_constraint( 3, [1, 4]);
constraint row_constraint( 4, [1, 3]);
constraint row_constraint( 5, [1, 4]);
constraint row_constraint( 6, [8]);
constraint row_constraint( 7, [7]);
constraint row_constraint( 8, [6]);
constraint row_constraint( 9, [4]);
constraint row_constraint(10, [2]);

constraint col_constraint( 1, [5]);
constraint col_constraint( 2, [3]);
constraint col_constraint( 3, [4]);
constraint col_constraint( 4, [5]);
constraint col_constraint( 5, [6]);
constraint col_constraint( 6, [6]);
constraint col_constraint( 7, [8]);
constraint col_constraint( 8, [1, 4]);
constraint col_constraint( 9, [2]);
constraint col_constraint(10, [1]);
