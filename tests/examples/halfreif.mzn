% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip

%% This is to test half-reification
%% Gleb Belov, 2019

var 0..10: x;
var 0..11: y;

var bool: b1;
var bool: b2;

constraint b1 -> x<=4;
constraint b2 -> y>=7;

constraint b1 \/ b2;

solve minimize x+b1-y;
