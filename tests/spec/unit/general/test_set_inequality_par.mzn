/***
!Test
expected: !Result
  status: SATISFIED
***/

% Test par evaluation of set inequality
array [int] of set of int: x = [{}, {1}, 1..2, 1..3, 1..4, {1,2,4}, {1,3}, {1,3,4}, {1,4}, {2}, 2..3, 2..4, {2,4}, {3}, 3..4, {4}];
int: n = length(x);

% Test every pair to ensure correct relationship
constraint forall (i, j in 1..n) (
  (i > j -> (x[i] > x[j]) /\ x[i] >= x[j]) /\
  (i < j -> (x[i] < x[j]) /\ x[i] <= x[j]) /\
  (i = j -> (x[i] <= x[j] /\ x[i] >= x[j]))
);
