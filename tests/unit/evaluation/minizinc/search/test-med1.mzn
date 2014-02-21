% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
%% Check that the indomain_median search complies with spec
%%

var 1..10: x;
var 2..10: y;
constraint x != y;

solve :: int_search([x,y],input_order,indomain_median,complete)
                satisfy;

output ["%% test-med1.mzn: output should be x = 5, y = 6\nx = ",
         show(x),";\ny = ",show(y),";\n"];
