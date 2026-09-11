% The constraint `set_of_int_inverse_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `set_of_int_inverse_reif` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

predicate fzn_set_of_int_inverse_reif(
  list of var set of int: f,
  int: f_offset,
  list of var set of int: invf,
  int: invf_offset,
  var bool: b,
) =
  b <-> (
    let {
      array [f_offset..f_offset + length(f) - 1] of var set of int: ff =
        array1d(f_offset..f_offset + length(f) - 1, f);
      array [invf_offset..invf_offset + length(invf) - 1] of var set of int: ii =
        array1d(invf_offset..invf_offset + length(invf) - 1, invf);
    } in forall (i in index_set(ff)) (ff[i] subset index_set(ii)) /\
      forall (j in index_set(ii)) (ii[j] subset index_set(ff)) /\
      forall (i in index_set(ff), j in index_set(ii)) (j in ff[i] <-> i in ii[j])
  );
