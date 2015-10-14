% RUNS ON mzn20_fd
% Test global ordering of symmetry predicates
% Global ordering is: variables in order from global_order predicate, followed
% by variables in order from the search strategy, followed by variables in
% order from any symmetry predicates defined in the model.

include "inverse.mzn";
include "var_sym.mzn";

% x is a 'variable' array of the numbers 1..100
int: n = 20;
array[1..n] of var 1..n: x;
array[1..n] of var 1..n: invx;
constraint inverse(x, invx);

% extend with bool array
array[1..n] of var bool: y;
constraint y[6];
constraint var_sym(y);

% Add variables to the global ordering
predicate add_to_global_order(array[int] of var int: a) :: symmetry = false;
predicate add_to_global_order_ord(array[int] of var int: a,
                                  array[int] of var int: order) = true;

% Setup some variables for the global ordering
constraint add_to_global_order([x[1], x[20], x[6], x[10], x[9], x[7], x[12]]);

solve
% Define a complicated and incomplete search strategy
:: seq_search([
      seq_search([
        int_search([x[4], x[5], x[8]], input_order, indomain_min, complete),
        int_search([x[2], x[1], x[5]], input_order, indomain_min, complete),
      ]),
      int_search([x[3], x[7], x[15]], input_order, indomain_min, complete),
      seq_search([
        seq_search([
          seq_search([
            int_search([x[9], x[10]], input_order, indomain_min, complete)
          ])
        ])
      ]),
      int_search([x[10], x[8]], input_order, indomain_min, complete),
      int_search([x[18], x[9]], input_order, indomain_min, complete)
    ])
% Define a similarly incomplete global ordering
:: global_order([x[8], x[12], x[2], x[1], x[13], x[17], x[7], x[2], y[4], y[2]])
  satisfy;

% Add a few more variables to the global ordering
constraint add_to_global_order([x[19], x[14], y[6], y[1]]);

% Constrain x using var sym, implicity adds a few more variables
constraint var_sym(x);

output [
  "global order is ", show([ invx[i] | i in 1..n ] ++ y)
];
