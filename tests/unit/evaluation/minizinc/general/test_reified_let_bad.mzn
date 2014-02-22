% RUNS ON mzn20_fd
var 1..10: x;
var bool: b;
constraint b <-> ( let { var 1..10: y } in x = y );
solve satisfy;
