/***
!Test
expected:
- !Result
  solution: !Solution
    x: []
***/

% Regression test: the frontend was not pushing the coercions into *both*
% arguments of the array1d cast.  This was leading to mzn2fzn to fail
% because the types in the assignment did not match.

array[1..0] of var int: x :: add_to_output = array1d({}, []);
solve satisfy;
