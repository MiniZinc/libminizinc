% 为校园游乐会做蛋糕

var 0..100: b; % 香蕉蛋糕的个数
var 0..100: c; % 巧克力蛋糕的个数

% 面粉
constraint 250*b + 200*c <= 4000;
% 香蕉
constraint 2*b  <= 6;
% 糖
constraint 75*b + 150*c <= 2000;
% 黄油
constraint 100*b + 150*c <= 500;
% 可可粉
constraint 75*c <= 500;

% 最大化我们的利润
solve maximize 400*b + 450*c;

output ["no. of banana cakes = \(b)\n",
         "no. of chocolate cakes = \(c)\n"];
