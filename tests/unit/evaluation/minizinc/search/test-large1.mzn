% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
%% Check that the largest search complies with spec
%%

var 1..10: x;
var 2..10: y;
constraint y <= 6 -> y > 6;
constraint x + y <= 13;

solve :: int_search([x,y],largest,indomain_split,complete)
                maximize x + y;

output ["%% test-med1.mzn: output should be x = 5, y = 8\nx = ",
         show(x),";\ny = ",show(y),";\n"];
