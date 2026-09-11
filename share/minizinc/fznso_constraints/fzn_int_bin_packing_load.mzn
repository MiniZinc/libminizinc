% The constraint `int_bin_packing_load`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_bin_packing_load` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% The bins are numbered by `load`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from is passed explicitly and the arrays
% are declared `list of` — one-based, as the type-checker checks.

predicate fzn_int_bin_packing_load(
  list of var int: bin,
  list of int: weight,
  list of var int: load,
  int: offset,
) =
  sum(load) = sum(weight) /\
    forall (i in index_set(bin)) (offset <= bin[i] /\ bin[i] <= offset + length(load) - 1) /\
    forall (b in index_set(load)) (
      load[b] = sum (i in index_set(bin)) (weight[i] * (bin[i] = offset + b - 1))
    );
