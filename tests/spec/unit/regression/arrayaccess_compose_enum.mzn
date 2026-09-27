/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    a: [1, 2, 3]
    i: 2
    j: 1
    r: 3
***/

% Composing a[b[i, j]] into a 2-dimensional array must not keep the enum type information of
% the 1-dimensional array a (debug assertion "typeId() == 0 || dim() == d")
enum E = {A, B, C};
array [E] of var 1..5: a;
array [1..2, 1..2] of E: b = [| A, B | C, A |];
var 1..2: i;
var 1..2: j;
var int: r :: output = a[b[i, j]];
constraint a = [1, 2, 3];
constraint r = 3;
