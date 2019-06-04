/** @group globals.channeling
  Constrains two arrays of set of int variables, \a f and \a invf, so that
  a \p j in f[\p i] iff \p i in invf[\p j].  All the values in each array's sets
  must be within the index set of the other array.
*/
predicate fzn_inverse_set(array[int] of var set of int: f,
                      array[int] of var set of int: invf) =
    forall(i in index_set(f)) (
           f[i] subset index_set(invf)
    ) /\
    forall(j in index_set(invf)) (
        invf[j] subset index_set(f)
    ) /\
    forall(i in index_set(f), j in index_set(invf)) (
        (j in f[i] <-> i in invf[j])
    );
