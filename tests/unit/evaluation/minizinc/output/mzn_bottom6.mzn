% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

solve satisfy;
output [show(array3d(1..2,1..2,1..2,[_,_,_,_,_,_,_,_])), "\n"];
