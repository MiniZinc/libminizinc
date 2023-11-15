include "ide/vis.mzn";

int: n = 4;
var 1..3: x;
var 1..3: y;
constraint x + y > n;

output :: vis_server("foo.html", (n: n)) (x: x, y: y);
