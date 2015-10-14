% Test index_sort: sorting variable arrays by their internal ids

% x is a 'variable' array of the numbers 1..10
int: n = 10;
array[1..n] of var 1..n: x;
constraint forall(i in 1..n-1)(x[i] < x[i + 1]);

% rearrange x in a few different ways
array[1..n] of int: p1 = [ 8, 6, 3, 10, 2, 9, 5, 1, 4, 7 ];
array[1..n] of int: p2 = [ 1, 2, 3, 4, 5, 10, 9, 8, 7, 6 ];
array[1..n] of int: p3 = [ 1, 3, 5, 7, 9, 2, 8, 4, 6, 10 ];
array[1..n] of int: p4 = [ 10, 4, 5, 2, 8, 1, 9, 7, 6, 3 ];

array[1..n] of var 1..n: x1 = [ x[p1[i]] | i in 1..n ];
array[1..n] of var 1..n: x2 = [ x[p2[i]] | i in 1..n ];
array[1..n] of var 1..n: x3 = [ x[p3[i]] | i in 1..n ];
array[1..n] of var 1..n: x4 = [ x[p4[i]] | i in 1..n ];

% sorting by x should give x, pi is the permutation required to sort xi
constraint p1 = index_sort(x1, x);
constraint p2 = index_sort(x2, x);
constraint p3 = index_sort(x3, x);
constraint p4 = index_sort(x4, x);

% various sorts; stability means that variables not occuring in the order arg
% should keep the same order as in the first argument
array[1..n] of var 1..n: y1 = index_sort(x1, x2);
array[1..n] of var 1..n: y2 = index_sort(x3, x2);
array[1..n] of var 1..n: y3 = index_sort(x, [x[4], x[6], x[8]]);
array[1..n] of var 1..n: y4 = index_sort(x1, [x[4], x[6], x[8]]);
array[1..n] of var 1..n: y5 = index_sort(x3, [x[4], x[6], x[8]]);
array[1..n] of var 1..n: y6 = index_sort(x4, [x[4], x[6], x[8]]);
array[1..n] of var 1..n: y7 = index_sort(x2, [x[8], x[1], x[2], x[5]]);
array[1..n] of var 1..n: y8 = index_sort(x4, [x[8], x[1], x[2], x[5]]);

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
