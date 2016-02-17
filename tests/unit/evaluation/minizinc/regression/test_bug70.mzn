% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd

var int: killed;
constraint killed = bool2int(eFailureModes = 1);

var int: eFailureModes;
constraint eFailureModes = 1;

solve satisfy;