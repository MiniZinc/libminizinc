int: n;
array [1..n] of var 1..n: q; % i列的皇后在行q[i]

include "alldifferent.mzn";

constraint alldifferent(q);                       % 不同行
constraint alldifferent([ q[i] + i | i in 1..n]); % 不同对角线
constraint alldifferent([ q[i] - i | i in 1..n]); % 上+下

% 搜索
solve :: int_search(q, first_fail, indomain_min, complete)
      satisfy;
output [ if fix(q[j]) == i then "Q" else "." endif ++
         if j == n then "\n" else "" endif | i,j in 1..n]
