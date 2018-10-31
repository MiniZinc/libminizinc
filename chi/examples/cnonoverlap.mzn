float: width;          % 包含圆的长方形的宽
float: height;         % 包含圆的长方形的高
float: r1;
var r1..width-r1:  x1; % (x1,y1) 是第一个圆的中心
var r1..height-r1: y1;
float: r2;
var r2..width-r2:  x2; % (x2,y2) 是第二个圆的中心
var r2..height-r2: y2;
                       % 中心之间至少有r1 + r2的距离
constraint (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) >= (r1+r2)*(r1+r2);
solve satisfy;
