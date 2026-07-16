include "fzn_disjunctive_strict_opt_decomp.mzn";

predicate fzn_disjunctive_strict_opt(
  array [int] of var opt int: start,
  array [int] of var int: duration,
) = fzn_disjunctive_strict_opt_decomp(start, duration);
