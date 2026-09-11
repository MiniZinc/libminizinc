% The constraint `int_inverse_in_range`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_inverse_in_range` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% See `fzn_inverse.mzn`. This is the partial form: an index outside the other
% array's range is allowed, and only says nothing rather than failing.

include "global_cardinality.mzn";

predicate fzn_int_inverse_in_range(
  list of var int: f,
  int: f_offset,
  list of var int: invf,
  int: invf_offset,
) =
  let {
    array [f_offset..f_offset + length(f) - 1] of var int: ff =
      array1d(f_offset..f_offset + length(f) - 1, f);
    array [invf_offset..invf_offset + length(invf) - 1] of var int: ii =
      array1d(invf_offset..invf_offset + length(invf) - 1, invf);
  } in forall (i in index_set(ff)) (ff[i] in index_set(ii) -> ii[ff[i]] = i) /\
    forall (j in index_set(ii)) (ii[j] in index_set(ff) -> ff[ii[j]] = j) /\
    redundant_constraint(strengthen_injection_for_inverse_in_range(ff, ii)) /\
    redundant_constraint(strengthen_injection_for_inverse_in_range(ii, ff));

predicate strengthen_injection_for_inverse_in_range(
  array [int] of var int: f,
  array [int] of var int: invf,
) =
  let {
    set of int: sinvf = {i | i in index_set(invf) where dom(invf[i]) subset index_set(f)};
  } in global_cardinality(f, [i | i in sinvf], [1 | i in sinvf]);
