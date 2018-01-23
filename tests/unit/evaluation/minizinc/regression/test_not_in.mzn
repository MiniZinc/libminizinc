% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip

var {2,4}: bla;
constraint not (bla in {1,3});
solve satisfy;
