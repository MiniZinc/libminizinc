% The constraint `int_cumulatives_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_cumulatives_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

include "fzn_cumulatives_decomp.mzn";

predicate fzn_int_cumulatives_reif(
  list of var int: start,
  list of var int: duration,
  list of var int: resource,
  list of var int: machine,
  list of var int: capacity,
  int: capacity_offset,
  bool: is_upper_bound,
  var bool: b,
) =
  b <-> (
    fzn_cumulatives_decomp(
      start,
      duration,
      resource,
      machine,
      array1d(capacity_offset..capacity_offset + length(capacity) - 1, capacity),
      is_upper_bound,
    )
  );
