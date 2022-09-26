/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

predicate lib_pred($T: x);

% Should be a valid overload since previous lib_pred has no body 
predicate lib_pred($T: x) = true;