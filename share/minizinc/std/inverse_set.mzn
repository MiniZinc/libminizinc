/** @group globals.channeling
  Constrains two arrays of set of int variables, \a f and \a invf, so that
  a \p j in f[\p i] iff \p i in invf[\p j].  All the values in each array's sets
  must be within the index set of the other array.
*/
predicate inverse_set(array[int] of var set of int: f,
                      array[int] of var set of int: invf) =
    fzn_inverse_set(f,invf);

include "fzn_inverse_set.mzn";
