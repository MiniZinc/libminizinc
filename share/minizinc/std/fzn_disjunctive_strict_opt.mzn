include "fzn_disjunctive_strict_opt_decomp.mzn";

predicate fzn_disjunctive_strict_opt(array[int] of var opt int: s,
                                     array[int] of var int: d) =
        fzn_disjunctive_strict_opt_decomp(s,d);