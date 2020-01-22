include "fzn_inverse_in_range.mzn";
include "fzn_inverse_in_range_reif.mzn";

/** @group globals.channeling
  If the \a i th variable of the collection \a X is assigned to \a j and if \a j is greater than or equal to 1 and less than or equal to the number of items of the collection \a Y then the \a j th variable of the collection \a Y is assigned to \a i.

  Conversely, if the \a j th variable of the collection \a Y is assigned to \a i and if \a i is greater than or equal to 1 and less than or equal to the number of items of the collection \a X then the \a i th variable of the collection \a X is assigned to \a j.
*/
predicate inverse_in_range(array[int] of var int: f,
                  array[int] of var int: invf) =
    fzn_inverse_in_range(f, invf);

predicate inverse_in_range_reif(array[int] of var int: f,
                       array[int] of var int: invf,
                       var bool: b) =
    fzn_inverse_in_range_reif(f, invf, b);
