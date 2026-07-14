predicate fzn_seq_precede_chain_set(array [int] of var set of int: xs) =
  if length(xs) = 0 then
    true
  else
    let {
      set of int: S = ub_array(xs); % set of possible values
      int: l = min(S); % least possible value
      int: u = max(S); % greatest possible value
      int: f = min(index_set(xs));
      array [index_set(xs)] of var l..u: H;
    } in H[f] <= 1 /\
      H[f] = max(xs[f] union {0}) /\
      forall (i in index_set(xs) diff {f}) (H[i] <= H[i - 1] + 1 /\ H[i] = max(max(xs[i]), H[i - 1]))
  endif;
