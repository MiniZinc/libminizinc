include "fzn_inverse.mzn";
include "analyse_alldifferent.mzn";

/** @group globals.channeling
  Constrains two arrays of int variables, \a f and \a invf, to represent
  inverse functions.  All the values in each array must be within the index
  set of the other array.
*/
predicate inverse(array[int] of var int: f,
                  array[int] of var int: invf) =
    analyse_alldifferent(f) /\
    analyse_alldifferent(invf) /\
    fzn_inverse(f, invf);
