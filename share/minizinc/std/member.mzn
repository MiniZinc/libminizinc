include "fzn_member_bool.mzn";
include "fzn_member_float.mzn";
include "fzn_member_int.mzn";
include "fzn_member_set.mzn";
include "fzn_set_member.mzn";

/** @group globals
  Requires that \a y occurs in the array \a x.
*/
predicate member(array[int] of var bool: x, var bool: y) =
    fzn_member_bool(x, y);

/** @group globals
  Requires that \a y occurs in the array \a x.
*/
predicate member(array[int] of var float: x, var float: y) =
    fzn_member_float(x, y);

/** @group globals
  Requires that \a y occurs in the array \a x.
*/
predicate member(array[int] of var int: x, var int: y) =
    fzn_member_int(x, y);

/** @group globals
  Requires that \a y occurs in the array \a x.
*/
predicate member(array[int] of var set of int: x, var set of int: y) =
    fzn_member_set(x, y);

/** @group globals
  Requires that \a y occurs in the set \a x.
*/
predicate member(var set of int: x, var int: y) =
    fzn_set_member(x, y);

