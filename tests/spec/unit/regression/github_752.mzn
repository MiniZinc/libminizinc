/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

% Regression test for GitHub #752

include "globals.mzn";

int: N=5;
array[1..5] of var 1..10 : x;
enum STATE = {S1, S2, S3};
% DFA to define 12*1
array [STATE,1..2] of opt STATE: trans = [|     1:  2:
                                          | S1: S2, <>
                                          | S2: S3, S2
                                          | S3: <>, <>|];

constraint regular(x, trans, S1, {S3});
