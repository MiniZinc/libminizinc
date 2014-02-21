% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp

% Regression test for bug #70: set2array coercions were not supported.

predicate my_global_cardinality_low_up(array[int] of var int: x,
                                    array[int] of int: cover,
				    array[int] of int: lbound,
                                    array[int] of int: ubound) =
     forall(i in index_set(cover)) (
         sum(j in index_set(x)) ( bool2int(x[j] = cover[i]) )
         in lbound[i]..ubound[i]
     );


predicate myall_different(array[int] of var int: x) =
    my_global_cardinality_low_up(
        x, set2array(dom_array(x)), [0 | i in dom_array(x)], [1 | i in dom_array(x)]);

array[1..3] of var 1..4: x;

constraint myall_different(x);

solve :: int_search(x, input_order, indomain_min, complete) satisfy;

output ["x = ", show(x), ";\n"];
