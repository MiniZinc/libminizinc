% Test sort_by_var: sorting variable arrays by their internal ids

% x is a 'variable' array of the numbers 1..10
int: n = 10;
array[1..n] of var 1..n: x;
constraint forall(i in 1..n-1)(x[i] < x[i + 1]);

% rearrange x in a few different ways
array[1..n] of var 1..n: x1
  = [ if i >= n div 2 then x[n + (n div 2) - i] else x[i] endif | i in 1..n ];
array[1..n] of var 1..n: x2
   = let { array[1..n] of int: p = [ 8, 6, 3, 10, 2, 9, 5, 1, 4, 7 ]; }
  in [ x[p[i]] | i in 1..n ];
array[1..n] of var 1..n: x3
  = [ if i mod 2 == 0 then x[i div 2] else x[i div 2 + n div 2 + 1] endif
      | i in 1..n ];
array[1..n] of var 1..n: x4
  = [ if i mod 2 == 0 then x[n - i div 2 + 1] else x[n div 2 - i div 2] endif
      | i in 1..n ];

% sorting by x should always give x
constraint x = sort_by_var(x1, x);
constraint x = sort_by_var(x2, x);
constraint x = sort_by_var(x3, x);
constraint x = sort_by_var(x4, x);
constraint x1 = sort_by_var(x, x1);
constraint x1 = sort_by_var(x2, x1);
constraint x1 = sort_by_var(x3, x1);
constraint x1 = sort_by_var(x4, x1);
constraint x2 = sort_by_var(x, x2);
constraint x2 = sort_by_var(x1, x2);
constraint x2 = sort_by_var(x3, x2);
constraint x2 = sort_by_var(x4, x2);
constraint x3 = sort_by_var(x, x3);
constraint x3 = sort_by_var(x1, x3);
constraint x3 = sort_by_var(x2, x3);
constraint x3 = sort_by_var(x4, x3);
constraint x4 = sort_by_var(x, x4);
constraint x4 = sort_by_var(x1, x4);
constraint x4 = sort_by_var(x2, x4);
constraint x4 = sort_by_var(x3, x4);

% various sorts; stability means that variables not occuring in the order arg
% should keep the same order as in the first argument
array[1..n] of var 1..n: y1 = sort_by_var(x1, x2);
array[1..n] of var 1..n: y2 = sort_by_var(x3, x4);
array[1..n] of var 1..n: y3 = sort_by_var(x, [x[4], x[6], x[8]]);
array[1..n] of var 1..n: y4 = sort_by_var(x1, [x[4], x[6], x[8]]);
array[1..n] of var 1..n: y5 = sort_by_var(x3, [x[4], x[6], x[8]]);
array[1..n] of var 1..n: y6 = sort_by_var(x4, [x[4], x[6], x[8]]);
array[1..n] of var 1..n: y7 = sort_by_var(x2, [x[8], x[1], x[2], x[5]]);
array[1..n] of var 1..n: y8 = sort_by_var(x4, [x[8], x[1], x[2], x[5]]);

solve satisfy;
output [
  show(y1), "\n",
  show(y2), "\n",
  show(y3), "\n",
  show(y4), "\n",
  show(y5), "\n",
  show(y6), "\n",
  show(y7), "\n",
  show(y8)
];
