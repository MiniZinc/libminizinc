predicate fzn_seq_precede_chain_int(array [int] of var int: xs) =
  if length(xs) = 0 then
    true
  else
    let {
      int: l = lb_array(xs); % least possible value
      int: u = ub_array(xs); % greatest possible value
      int: f = min(index_set(xs));
      array [index_set(xs)] of var l..u: H;
    } in H[f] <= 1 /\
      H[f] = max(xs[f], 0) /\
      forall (i in index_set(xs) diff {f}) (H[i] <= H[i - 1] + 1 /\ H[i] = max(xs[i], H[i - 1]))
  endif;
