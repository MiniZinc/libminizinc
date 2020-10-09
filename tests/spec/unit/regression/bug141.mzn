/***
!Test
expected:
- !Result
  solution: !Solution
    x:
    - 1
    - 1
    - 1
    - 1
    - 1
    - 1
    - 1
    - 1
    - 1
- !Result
  solution: !Solution
    x:
    - 9
    - 9
    - 9
    - 9
    - 9
    - 9
    - 9
    - 9
    - 9
***/

% Regression test for bug #141.
% In mzn2fzn 1.1.5 'x' was not being marked as an output variable in the
% generated FlatZinc because the code that processed output items was
% ignoring the assignments for any let variables.

array[1..9] of var 1..9: x;

solve satisfy;

output
    let { array[1..9] of 1..99: y =
           [ sum(s2 in 1..9)( fix(x[s2]) ) | s in 1..9 ] }
    in
    [ show(y) ];
