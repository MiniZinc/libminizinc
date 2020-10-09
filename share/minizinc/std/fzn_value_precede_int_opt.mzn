predicate fzn_value_precede_int_opt(int: s, int: t, array[int] of var opt int: x) =
  value_precede_int(s,t,[if occurs(y) then deopt(y) else max(s,t)+1 endif | y in x]);
