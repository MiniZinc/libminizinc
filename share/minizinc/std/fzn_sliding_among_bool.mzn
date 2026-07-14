include "sliding_sum.mzn";

predicate fzn_sliding_among_bool(int: low, int: up, int: window_size, array [int] of var bool: xs, bool: value) =
    sliding_sum(low,up,window_size,[xi=value | xi in xs]);
