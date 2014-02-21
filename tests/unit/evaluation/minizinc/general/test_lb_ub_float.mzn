% RUNS ON mzn_mer_lp
% RUNS ON mzn_mer_fdlp

var 1.0..10.0: x;

var float: test_1 = lb(3.0);
var float: test_2 = ub(3.0);

var float: test_3 = lb(x);
var float: test_4 = ub(x);

solve satisfy;

output [
    "test_1 = ", show(test_1), ";\n",
    "test_2 = ", show(test_2), ";\n",
    "test_3 = ", show(test_3), ";\n",
    "test_4 = ", show(test_4), ";\n"
];
