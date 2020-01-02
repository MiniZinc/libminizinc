% magicsq.mzn.model
% vim: ft=zinc ts=4 sw=4 et tw=0
%
% A model for magic square problems.

include "globals.mzn";

int: n;                         % The length of side of the square.

int: s = (n * (n * n + 1)) div 2;
                                % The Magic Constant - see
                                % http://mathworld.wolfram.com/MagicSquare.html

array [1..n, 1..n] of var 1..(n * n): a;

% Every square must have a different value.

constraint all_different (r, c in 1..n) (a[r, c]);

% Every row, column, and major diagonal must sum to the Magic Constant.

constraint forall (c in 1..n) (sum (r in 1..n) (a[r, c]) = s);
constraint forall (r in 1..n) (sum (c in 1..n) (a[r, c]) = s);
constraint sum (i in 1..n) (a[i, i]) = s;
constraint sum (i in 1..n) (a[i, n + 1 - i]) = s;

% Any solution will do.

solve satisfy;

output [
     show_int(floor(log10(int2float(n * n))) + 1, a[r, c]) ++ 
     if c = n then "\n" else " " endif
     | r, c in 1..n
];
