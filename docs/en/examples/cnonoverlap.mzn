float: width;          % width of rectangle to hold circles
float: height;         % height of rectangle to hold circles
float: r1;
var r1..width-r1:  x1; % (x1,y1) is center of circle of radius r1
var r1..height-r1: y1;
float: r2;
var r2..width-r2:  x2; % (x2,y2) is center of circle of radius r2
var r2..height-r2: y2;
                       % centers are at least r1 + r2 apart
constraint (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) >= (r1+r2)*(r1+r2);
solve satisfy;
