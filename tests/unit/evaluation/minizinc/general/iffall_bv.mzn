% RUNS ON mzn20_fd

constraint iffall([]) = true;
constraint iffall([false]) = true;
constraint iffall([true]) = false;
constraint iffall([false, true]) = false;
constraint iffall([true, false]) = false;
constraint iffall([true, true]) = true;
constraint iffall([false, false]) = true;

solve satisfy;
output ["Ok\n"];
