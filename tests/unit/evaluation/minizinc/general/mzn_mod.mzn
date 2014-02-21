% RUNS ON mzn_mer_fd

% Test for MiniZinc/FlatZinc 1.1 semantics for mod.

constraint  7  mod  4 =  3;
constraint -7  mod  4 = -3;
constraint  7  mod -4 =  3;
constraint -7  mod -4 = -3; 

solve satisfy;

