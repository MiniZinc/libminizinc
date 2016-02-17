% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
annotation simpleann;
annotation composeann(ann: s) = seq_search([s,s]);
function ann: annfunc(ann: s) = composeann(s);
solve :: annfunc(composeann(simpleann)) satisfy;
