% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
%% Check that the first-fail search complies with spec
%%

var 1..8: x;
var 1..10: y;
var 1..10: z;
var 1..10: w;    
constraint x = 1 -> x < 1;
constraint x > 1 -> y < 5;
constraint x > 1 -> z < 5;
constraint x > 1 -> w < 5;
constraint y = 1 -> x > 4;

solve :: int_search([x,y,z,w],first_fail,indomain_min,complete)
                satisfy;

output ["%% test-ff2.mzn: output should be x = 5, y = 1\nx = ",
         show(x),";\ny = ",show(y),";\n"];
