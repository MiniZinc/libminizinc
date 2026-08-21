% The constraint `int_bin_packing`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_bin_packing` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% The bins are numbered by `capacity`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from is passed explicitly and the arrays are
% declared `list of` — one-based, as the type-checker checks.

predicate fzn_int_bin_packing(
  list of var int: bin,
  list of int: weight,
  list of int: capacity,
  int: offset,
) =
  forall (i in index_set(bin)) (offset <= bin[i] /\ bin[i] <= offset + length(capacity) - 1) /\
    forall (k in index_set(capacity)) (
      capacity[k] >= sum (i in index_set(bin)) (weight[i] * (bin[i] = offset + k - 1))
    );
