% RUNS ON mzn_mer_fd

% Test for MiniZinc/FlatZinc 1.1 semantics for division.

constraint  7  div  4 =  1;
constraint -7  div  4 = -1;
constraint  7  div -4 = -1;
constraint -7  div -4 =  1;

solve satisfy;
