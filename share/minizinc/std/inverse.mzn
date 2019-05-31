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

include "fzn_inverse.mzn";
include "analyse_alldifferent.mzn";

/** @group globals.channeling
  Given a function \a f represented as an array, return the inverse function.
*/
function array[int] of var int: inverse(array[int] of var int: f) =
    let {
      array[lb_array(f)..ub_array(f)] of var index_set(f): invf;
      constraint inverse(f,invf);
    } in invf;
