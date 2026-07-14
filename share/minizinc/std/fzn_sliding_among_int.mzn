include "sliding_sum.mzn";

predicate fzn_sliding_among_int(int: low, int: up, int: window_size, array [int] of var int: xs, set of int: values) =
    sliding_sum(low,up,window_size,[xi in values | xi in xs]);
