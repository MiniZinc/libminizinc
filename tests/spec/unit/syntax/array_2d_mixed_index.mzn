/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

% Indexing only some rows leaves the row header shorter than the rows, which
% used to silently reshape the literal (here 2x2 became 1x4).
array[int,int] of int: a = [| 1, 2 | 3: 3, 4 |];
solve satisfy;
