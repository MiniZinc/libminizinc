% 为校园游乐会做蛋糕（和数据文件一起）

int: flour;  %拥有的面粉克数
int: banana; %拥有的香蕉个数
int: sugar;  %拥有的糖克数
int: butter; %拥有的黄油克数
int: cocoa;  %拥有的可可粉克数

constraint assert(flour >= 0,"Invalid datafile: " ++
                  "Amount of flour should be non-negative");
constraint assert(banana >= 0,"Invalid datafile: " ++
                  "Amount of banana should be non-negative");
constraint assert(sugar >= 0,"Invalid datafile: " ++
                  "Amount of sugar should be non-negative");
constraint assert(butter >= 0,"Invalid datafile: " ++
                  "Amount of butter should be non-negative");
constraint assert(cocoa >= 0,"Invalid datafile: " ++
                  "Amount of cocoa should be non-negative");

var 0..100: b; % 香蕉蛋糕的个数
var 0..100: c; % 巧克力蛋糕的个数

% 面粉
constraint 250*b + 200*c <= flour;
% 香蕉
constraint 2*b  <= banana;
% 糖
constraint 75*b + 150*c <= sugar;
% 黄油
constraint 100*b + 150*c <= butter;
% 可可粉
constraint 75*c <= cocoa;

% 最大化我们的利润
solve maximize 400*b + 450*c;

output ["no. of banana cakes = \(b)\n",
        "no. of chocolate cakes = \(c)\n"];
