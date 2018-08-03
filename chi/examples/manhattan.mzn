int: n;
set of int: NUM = 1..n;

array[NUM] of var NUM: x;
array[NUM] of var NUM: y;
array[NUM,NUM] of var 0..2*n-2: dist = 
     array2d(NUM,NUM,[ 
     if i < j then manhattan(x[i],y[i],x[j],y[j]) else 0 endif
                     | i,j in NUM ]);

% manf
function var int: manhattan(var int: x1, var int: y1,
                            var int: x2, var int: y2) =
         abs(x1 - x2) + abs(y1 - y2);

constraint forall(i,j in NUM where i < j)
                 (dist[i,j] >= max(i,j)-1);

var int: obj = sum(i,j in NUM where i < j)(dist[i,j]);
solve minimize obj; 

% 简单地显示结果
include "alldifferent_except_0.mzn";
array[NUM,NUM] of var 0..n: grid;
constraint forall(i in NUM)(grid[x[i],y[i]] = i);
constraint alldifferent_except_0([grid[i,j] | i,j in NUM]);

output ["obj = \(obj);\n"] ++
       [ if fix(grid[i,j]) > 0 then show(grid[i,j]) else "." endif
         ++ if j = n  then "\n" else "" endif 
       | i,j in NUM ]; 
