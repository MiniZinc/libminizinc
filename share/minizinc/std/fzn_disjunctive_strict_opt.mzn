include "fzn_disjunctive_strict_opt_decomp.mzn";

predicate fzn_disjunctive_strict_opt(
  array [int] of var opt int: starts,
  array [int] of var int: durations,
) = fzn_disjunctive_strict_opt_decomp(starts, durations);
