/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution {}
***/

% Two predicates with identical parameter types but different parameter names
% must coexist (named-arguments step 3). They are not called from this model;
% positional call resolution against this pair would be ambiguous and is the
% subject of later named-args steps.

predicate interval(int: start, int: duration) = true;
predicate interval(int: start, int: end) = true;

solve satisfy;
