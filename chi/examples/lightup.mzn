int: h; set of int: H = 1..h; % 板高度
int: w; set of int: W = 1..w; % 板宽度
array[H,W] of -1..5: b;       % 板
int: E = -1;                  % 空白格
set of int: N = 0..4;         % 填充并含有数字的网格
int: F =  5;                  % 填充但不含有数字的网格

% 位置 (i1,j1) 对位置 (i2,j2) 可见 
test visible(int: i1, int: j1, int: i2, int: j2) = 
    ((i1 == i2) /\ forall(j in min(j1,j2)..max(j1,j2))(b[i1,j] == E))
 \/ ((j1 == j2) /\ forall(i in min(i1,i2)..max(i1,i2))(b[i,j1] == E));

array[H,W] of var bool: l; % is there a light

% 填充的网格没有灯泡
constraint forall(i in H, j in W, where b[i,j] != E)(l[i,j] == false);
% lights next to filled numbered square agree
include "boolsum.mzn";
constraint forall(i in H, j in W where b[i,j] in N)(
    bool_sum_eq([ l[i1,j1] | i1 in i-1..i+1, j1 in j-1..j+1 where
                             abs(i1 - i) + abs(j1 - j) == 1 /\
                             i1 in H /\ j1 in W ], b[i,j]));
% 每个空白网格是被照亮的
constraint forall(i in H, j in W where b[i,j] == E)(
             exists(j1 in W where visible(i,j,i,j1))(l[i,j1]) \/
             exists(i1 in H where visible(i,j,i1,j))(l[i1,j]) 
           );
% 任何两个灯泡看不到彼此
constraint forall(i1,i2 in H, j1,j2 in W where 
                  (i1 != i2 \/ j1 != j2) /\ b[i1,j1] == E 
                  /\ b[i2,j2] == E /\ visible(i1,j1,i2,j2))(
             not l[i1,j1] \/ not l[i2,j2]
           );

solve satisfy;
output [ if b[i,j] != E then show(b[i,j])
         else if fix(l[i,j]) then "L" else "." endif
         endif ++ if j == w then "\n" else " " endif |
         i in H, j in W];
