% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd

var set of 1..1: s1;
var set of 1..3: s2;

constraint s2 = s1 union if 1 in s1 then {2} else {3} endif;

solve satisfy;