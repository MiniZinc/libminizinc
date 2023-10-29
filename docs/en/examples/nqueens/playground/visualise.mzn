include "ide/vis.mzn";
constraint ide_launch_server("nqueens.html", n);
output :: "vis_json" [showJSON([i - 1| i in q])];
