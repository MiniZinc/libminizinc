include "all_different.mzn";

predicate fzn_inverse_opt(array[int] of var opt int: f,
                          array[int] of var opt int: invf) =
  all_different(f) /\
  all_different(invf) /\
  forall (i in index_set(f)) (
    occurs(f[i]) <-> invf[f[i]] = i
  ) /\
  forall (j in index_set(invf)) (
    occurs(invf[j]) <-> f[invf[j]] = j
  );
