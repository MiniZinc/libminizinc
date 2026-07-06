/***
!Test
solvers: [gecode]
expected:
- !Result
  status: UNSATISFIABLE
***/

% Regression test: assigning a fixed set value to a set variable via a
% root-context equality must fail when the value contains an element outside
% the variable's declared universe. Previously the out-of-universe element was
% silently dropped when computing the variable's domain, and the assignment
% (folding S to {1,3}) was applied without checking it against that domain, so
% the model was wrongly reported satisfiable.
%
% Here S can only ever be a subset of {1,2}, so S = {1,3} (which requires the
% out-of-universe element 3) is UNSATISFIABLE.
var set of 1..2: S;
constraint S = {1,3};
solve satisfy;
