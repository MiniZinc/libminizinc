include "cumulatives.mzn";

predicate fzn_cumulatives_opt(
  array [int] of var opt int: s,
  array [int] of var int: d,
  array [int] of var int: r,
  array [int] of var int: m,
  array [int] of var int: b,
  bool: upper,
  int: min_m,
) = cumulatives(
    [si default 0 | si in s],
    [if occurs(s[i]) then d[i] else 0 endif | i in index_set(s)],
    r, m, b, upper);
