% RUNS ON mzn20_fd
var 1..10: x;
constraint not( let { var 1..10: y } in x = y );
solve satisfy;
