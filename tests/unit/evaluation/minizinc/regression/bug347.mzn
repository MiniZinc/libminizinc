% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_std_fd
% RUNS ON minizinc_fd

% Regression test for bug #347: mzn2fzn's FlatZinc optimiser was incorrectly
% optimising away some of the equality constraints involving uav1 below.

include "alldifferent.mzn";                                                                                                                                               
var 0..6: uav1;
var 0..6: uav2;
var 0..6: uav3;
var 0..6: uav4;
var 0..6: execution_unit_1000_3;
var 0..6: execution_unit_1000_4;
constraint alldifferent([uav1, uav2, uav3, uav4]);
constraint execution_unit_1000_3 = uav1;
constraint execution_unit_1000_4 = uav1;
solve satisfy;
output [
    "uav1 = ", show(uav1), ";\n",
    "uav2 = ", show(uav2), ";\n",
    "uav3 = ", show(uav3), ";\n",
    "uav4 = ", show(uav4), ";\n"
];
