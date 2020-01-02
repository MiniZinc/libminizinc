/***
!Test
expected:
- !Result
  solution: !Solution
    b: [1, 1, 1]
- !Result
  solution: !Solution
    b: [1, 3, 3]
***/

% Regression test for the second problem in bug 68.
% The type checker was assigning the type var bottom to the elements of
% array1d(1..2, [_, _]) below and that was causing mzn2fzn to abort.

int: N = 3;
array[1..N] of var 1 .. N: b;
constraint forall([ b[i] = [1, _, _][i] | i in index_set(b) ]);
solve satisfy;
output ["b = ", show(b), "\n"];
