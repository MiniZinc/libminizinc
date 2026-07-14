include "fzn_inverse_in_range.mzn";
include "fzn_inverse_in_range_reif.mzn";

predicate fzn_inverse_in_range(array [int] of var int: xs, array [int] of var int: ys);

/** @group globals.channeling
  If the \a i th variable of the collection \a xs is assigned to \a j and if \a j is in the index set of \a ys then the \a j th variable of the collection \a ys is assigned to \a i.

  Conversely, if the \a j th variable of the collection \a ys is assigned to \a i and if \a i is in the index set of \a xs then the \a i th variable of the collection \a xs is assigned to \a j.
*/
predicate inverse_in_range(array [$$A] of var $$B: xs, array [$$B] of var $$A: ys) =
  fzn_inverse_in_range(index2int(enum2int(xs)), index2int(enum2int(ys)));
