/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

array [1..0] of var 1..2: x0;
array [1..2] of var 1..2: x1;
array [1..2, 1..2] of var 1..2: x2;
array [1..2, 1..2, 1..2] of var 1..2: x3;
array [1..2, 1..2, 1..2, 1..2] of var 1..2: x4;
array [1..2, 1..2, 1..2, 1..2, 1..2] of var 1..2: x5;
array [1..2, 1..2, 1..2, 1..2, 1..2, 1..2] of var 1..2: x6;

array [1..0] of var opt 1..2: y0;
array [1..2] of var opt 1..2: y1;
array [1..2, 1..2] of var opt 1..2: y2;
array [1..2, 1..2, 1..2] of var opt 1..2: y3;
array [1..2, 1..2, 1..2, 1..2] of var opt 1..2: y4;
array [1..2, 1..2, 1..2, 1..2, 1..2] of var opt 1..2: y5;
array [1..2, 1..2, 1..2, 1..2, 1..2, 1..2] of var opt 1..2: y6;

var 0..3: i;
any: vi :: output = [x1[i], x2[i, i], x3[i, i, i], x4[i, i, i, i], x5[i, i, i, i, i], x6[i, i, i, i, i, i]];
any: ui :: output = [y1[i], y2[i, i], y3[i, i, i], y4[i, i, i, i], y5[i, i, i, i, i], y6[i, i, i, i, i, i]];

var opt 0..3: j;
any: vj :: output = [x1[j], x2[i, j], x3[i, j, i], x4[i, j, i, j], x5[i, j, i, j, i], x6[i, j, i, j, i, j]];
any: uj :: output = [y1[j], y2[i, j], y3[i, j, i], y4[i, j, i, j], y5[i, j, i, j, i], y6[i, j, i, j, i, j]];

var opt 1..3: k;
any: vk :: output = x1[k];
any: uk :: output = y1[k];

var opt 1..2: l;
any: el :: output = x0[l];
any: fl :: output = y0[l];

constraint occurs(y1[1]) /\ occurs(y2[1, 1]) /\ occurs(y3[1, 1, 1]) /\ occurs(y4[1, 1, 1, 1]) /\ occurs(y5[1, 1, 1, 1, 1]) /\ occurs(y6[1, 1, 1, 1, 1, 1]);
solve minimize sum(x0 ++ x1 ++ array1d(x2) ++ array1d(x3) ++ array1d(x4) ++ array1d(x5) ++ array1d(x6) ++ y0 ++ y1 ++ array1d(y2) ++ array1d(y3) ++ array1d(y4) ++ array1d(y5) ++ array1d(y6));
