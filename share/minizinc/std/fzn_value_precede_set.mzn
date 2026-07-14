predicate fzn_value_precede_set(int: before, int: after, array [int] of var set of int: xs) =
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
          var bool: xis = (before in xs[i] /\ not (after in xs[i]));
        } in (xis -> (b[i + 1] == true)) /\
          ((not xis) -> (b[i] == b[i + 1])) /\
          ((not b[i]) -> (before in xs[i] \/ not (after in xs[i])))
      ) /\
        b[imin] == false
    )
  endif;
