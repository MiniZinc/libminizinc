int: w = 4;
int: h = 4;

% arraydec
set of int: HEIGHT = 0..h;
set of int: CHEIGHT = 1..h-1;
set of int: WIDTH = 0..w;
set of int: CWIDTH = 1..w-1;  
array[HEIGHT,WIDTH] of var float: t; % 在点（i,j）处的温度
var float: left;   % 左侧温度
var float: right;  % 右侧温度
var float: top;    % 顶部温度
var float: bottom; % 底部温度

% 拉普拉斯方程：每一个内部点温度是它相邻点的平均值
constraint forall(i in CHEIGHT, j in CWIDTH)(
              4.0*t[i,j] = t[i-1,j] + t[i,j-1] + t[i+1,j] + t[i,j+1]);

% sides
% 边约束
constraint forall(i in CHEIGHT)(t[i,0] = left);
constraint forall(i in CHEIGHT)(t[i,w] = right);
constraint forall(j in CWIDTH)(t[0,j] = top);
constraint forall(j in CWIDTH)(t[h,j] = bottom);

% 角约束
constraint t[0,0]=0.0;
constraint t[0,w]=0.0;
constraint t[h,0]=0.0;
constraint t[h,w]=0.0; 
left = 0.0;
right = 0.0;
top = 100.0;
bottom = 0.0;

solve satisfy;

output [ show_float(6, 2, t[i,j]) ++ 
         if j == h then "\n" else " " endif |
         i in HEIGHT, j in WIDTH
];
