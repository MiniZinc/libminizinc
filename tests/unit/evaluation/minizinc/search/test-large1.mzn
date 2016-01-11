% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
% RUNS ON mzn20_mip
%% Check that the largest search complies with spec
%%

var 1..10: x;
var 2..10: y;
constraint 7 < y \/ y > 6;
constraint x + y <= 13;

solve :: int_search([x,y],largest,indomain_split,complete)
                maximize x + y;

output ["%% test-med1.mzn: output should be x = 5, y = 8\nx = ",
         show(x),";\ny = ",show(y),";\n"];
