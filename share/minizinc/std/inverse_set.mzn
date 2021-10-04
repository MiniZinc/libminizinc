include "fzn_inverse_set.mzn";
include "fzn_inverse_set_reif.mzn";

/** @group globals.channeling
  Constrains two arrays of set of int variables, \a f and \a invf, so that
  a \p j in f[\p i] iff \p i in invf[\p j].  All the values in each array's sets
  must be within the index set of the other array.
*/
predicate inverse_set(
	array[$$X] of var set of $$Y: f,
	array[$$Y] of var set of $$X: invf
) = fzn_inverse_set(f,invf);
