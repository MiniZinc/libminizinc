/***
!Test
expected:
- !Result
  solution: !Solution
    objective: 337561
***/

% perfsq2.mzn
% vim: ft=zinc ts=4 sw=4 et
%
% Perfect squares: find a square equal to the sum of smaller, distinct squares.

int: n = 100;

% x[k] = 1 iff k^2 is part of the sum.
%
array [1..n] of var 0..1: x;

% t is the sum of the first n squares, the largest value our sum can have.
%
int: t = ((n * (n + 1) * (2 * n + 1)) div 6);

% squares is the set of square numbers less than t.
%
set of int: squares = {i * i | i in 1..(n * n) where i * i <= t};

% k is the sum of the squares selected by the x[i].
%
var squares: k = sum (i in 1..n) (i * i * x[i]);

solve maximize(k);

output [show(k), "\n"];

% output
%     [   show(k), "  =  sum of "
%     ] ++
%     [   if fix(x[i]) = 1 then show(i * i) ++ " " else "" endif
%     |   i in 1..n
%     ] ++
%     [   "\n"
%     ];
