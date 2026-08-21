% The constraint `int_inverse_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_inverse_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_int_inverse_reif(
  list of var int: f,
  int: f_offset,
  list of var int: invf,
  int: invf_offset,
  var bool: b,
) =
  b <-> (
    let {
      array [f_offset..f_offset + length(f) - 1] of var int: ff = array1d(f_offset..f_offset + length(f) - 1, f);
      array [invf_offset..invf_offset + length(invf) - 1] of var int: ii =
        array1d(invf_offset..invf_offset + length(invf) - 1, invf);
    } in forall (i in index_set(ff)) (ff[i] in index_set(ii) /\ ii[ff[i]] = i) /\
      forall (j in index_set(ii)) (ii[j] in index_set(ff) /\ ff[ii[j]] = j)
  );
