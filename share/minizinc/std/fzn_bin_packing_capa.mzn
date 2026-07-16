predicate fzn_bin_packing_capa(
  array [int] of int: capacity,
  array [int] of var int: bin,
  array [int] of int: weight,
) =
  forall (i in index_set(bin)) (
    min(index_set(capacity)) <= bin[i] /\ bin[i] <= max(index_set(capacity))
  ) /\
    forall (b in index_set(capacity)) (
      capacity[b] >= sum (i in index_set(bin)) (weight[i] * (bin[i] = b))
    );

%-----------------------------------------------------------------------------%
