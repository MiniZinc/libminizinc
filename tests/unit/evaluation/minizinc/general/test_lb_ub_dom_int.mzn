% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON mzn_mer_lazyfd

var 1..10: x;

var -100..100: test_1 = lb(3);
var -100..100: test_2 = ub(3);

var -100..100: test_3 = lb(x);
var -100..100: test_4 = ub(x);

constraint dom(x) = 1..10;

solve satisfy;

output [
   "test_1 = ", show(test_1), ";\n",
   "test_2 = ", show(test_2), ";\n",
   "test_3 = ", show(test_3), ";\n",
   "test_4 = ", show(test_4), ";\n"
];
