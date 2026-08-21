% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% Each array is one-based on the solver's side, so the index the caller numbers
% it from travels with it: `f[i] = j` and `invf[j] = i` are read in the caller's
% numbering, not in the flat array's.
%
% A solver meets an offset by padding the front of its own array, which it can
% only do from zero up; a caller numbering from below zero is left to the
% decomposition. The same holds for the other offset-taking predicates here.

include "fzn_int_inverse.mzn";

predicate fzn_inverse(array [int] of var int: f, array [int] of var int: invf) =
  if length(f) = 0 \/ length(invf) = 0 \/ min(index_set(f)) < 0 \/ min(index_set(invf)) < 0 then
    forall (i in index_set(f)) (f[i] in index_set(invf) /\ invf[f[i]] = i) /\
      forall (j in index_set(invf)) (invf[j] in index_set(f) /\ f[invf[j]] = j)
  else
    fzn_int_inverse(array1d(f), min(index_set(f)), array1d(invf), min(index_set(invf)))
  endif;
