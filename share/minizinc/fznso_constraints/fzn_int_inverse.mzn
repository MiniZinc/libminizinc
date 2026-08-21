% The constraint `int_inverse`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_inverse` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% Each array is one-based on the solver's side, so the index the caller numbers
% it from travels with it: `f[i] = j` and `invf[j] = i` are read in the caller's
% numbering, not in the flat array's.
%
% A solver meets an offset by padding the front of its own array, which it can
% only do from zero up; a caller numbering from below zero is left to the
% decomposition. The same holds for the other offset-taking predicates here.

predicate fzn_int_inverse(list of var int: f, int: f_offset, list of var int: invf, int: invf_offset) =
  let {
    array [f_offset..f_offset + length(f) - 1] of var int: ff = array1d(f_offset..f_offset + length(f) - 1, f);
    array [invf_offset..invf_offset + length(invf) - 1] of var int: ii =
      array1d(invf_offset..invf_offset + length(invf) - 1, invf);
  } in forall (i in index_set(ff)) (ff[i] in index_set(ii) /\ ii[ff[i]] = i) /\
    forall (j in index_set(ii)) (ii[j] in index_set(ff) /\ ff[ii[j]] = j);
