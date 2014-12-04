% RUNS ON mzn20_fd
% flipstrip
% miniimize the number of moves to turn all bools to true

array[1..1] of bool: pos = [false];

var 1..1: f1;

constraint false \/ not pos[f1];
 
solve satisfy;

output [ show(f1) ];
