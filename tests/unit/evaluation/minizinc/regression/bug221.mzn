% RUNS ON minizinc_fd

% Regression test for bug #221.
% In the G12/FD implementation of cumulative, ext_edge_finding_filtering
% required an e-feasibility check to be run, but specifying the above
% annotation alone did not imply that.

% This model is unsatisfiable.

include "cumulative.mzn" ;

int: size = 10;
array [1..2*size] of int: ones = [1 | Index in 1..2*size] ;
array [1..2*size] of var 1..(2*size-1): start;

constraint cumulative(start,ones,ones,1) :: ext_edge_finding_filtering ;

solve satisfy ;

output [show(start), "\n" ] ;
