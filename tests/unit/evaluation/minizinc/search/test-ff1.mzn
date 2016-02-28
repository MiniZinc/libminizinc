% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
%% Check that the first-fail search complies with spec
%%

var 1..8: x;
var 1..10: y;
constraint x > 1 -> y > 7;
constraint x = 1 -> y < 3;
constraint x + y <= 11;

solve :: int_search([y,x],first_fail,indomain_min,complete)
                maximize x + y;

output ["%% test-ff1.mzn: output should be x = 3, y = 8\nx = ",
         show(x),";\ny = ",show(y),";\n"];
