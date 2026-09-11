% The constraint `int_bin_packing_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_bin_packing_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_int_bin_packing_reif(
  list of var int: bin,
  list of int: weight,
  list of int: capacity,
  int: offset,
  var bool: b,
) =
  b <->
    (
      forall (i in index_set(bin)) (min(index_set(capacity)) <= bin[i] /\ bin[i] <= max(index_set(capacity))) /\
        forall (assigned_bin in index_set(capacity)) (
          capacity[assigned_bin] >= sum (i in index_set(bin)) (weight[i] * (bin[i] = assigned_bin))
        )
    );
