predicate fzn_value_precede_int(int: before, int: after, array [int] of var int: xs) =
  if length(xs) = 0 then
    true
  else
    let {
      int: imin = min(index_set(xs));
      int: imax = max(index_set(xs));
      array [imin..imax + 1] of var bool: b;
    } in (
      forall (i in imin..imax) (
        let {
          var bool: xis = (xs[i] == before);
        } in (xis -> (b[i + 1] == true)) /\
          ((not xis) -> (b[i] == b[i + 1])) /\
          ((not b[i]) -> (xs[i] != after))
      ) /\
        b[imin] == false
    )
  endif;
