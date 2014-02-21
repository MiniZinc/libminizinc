% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

test test1(1..10: x) = true;
solve satisfy;
output [ if test1(11) then "DID NOT WORK" else "WORKED" endif, "\n"];
