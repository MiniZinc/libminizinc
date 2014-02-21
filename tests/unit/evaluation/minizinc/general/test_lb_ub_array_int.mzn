% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON mzn_mer_lazyfd

array[1..3] of var 1..3: a;
var -100..100: test_1 = lb(a[1]);
var -100..100: test_2 = ub(a[1]);

constraint lb_array(a) = 1;
constraint ub_array(a) = 3;

solve satisfy;

output [
   "test_1 = ", show(test_1), ";\n",
   "test_2 = ", show(test_2), ";\n"
];
