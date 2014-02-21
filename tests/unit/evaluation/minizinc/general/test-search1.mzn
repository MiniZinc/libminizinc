% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
%% Check that the first-fail search complies with spec
%%

var 1..8: x;
var 1..10: y;
constraint x > 1 -> y > 7;
constraint x = 1 -> y < 3;
constraint x + y <= 11;

solve :: int_search([y,x],first_fail,indomain_min,complete)
                maximize x + y;

output ["x = ",show(x),";\ny = ",show(y),";\n%% should be x = 3, y = 8\n"];
