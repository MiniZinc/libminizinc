%-----------------------------------------------------------------------------%
% Requires that each item i with weight w[i], be put into bin[i] such
% that the sum of the weights of the items in each bin b is equal to load[b].
% Assumptions:
% - forall i, w[i] >= 0
%-----------------------------------------------------------------------------%

predicate ilogcp_bin_packing_load(array[int] of var int: load,
                           array[int] of var int: bin,
                           array[int] of int: w);

predicate bin_packing_load(array[int] of var int: load,
                           array[int] of var int: bin,
                           array[int] of int: w) =
    assert(index_set(bin) == index_set(w),
        "bin_packing_load: the bin and weight arrays must have identical index sets",
    assert(lb_array(w) >= 0,
        "bin_packing_load: the weights must be non-negative",
        ilogcp_bin_packing_load(load,bin,w)
    ));

%-----------------------------------------------------------------------------%
