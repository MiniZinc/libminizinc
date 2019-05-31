include "member_bool.mzn";
include "member_float.mzn";
include "member_int.mzn";
include "member_set.mzn";
include "set_member.mzn";

/** @group globals
  Requires that \a y occurs in the array \a x.
*/
predicate member(array[int] of var bool: x, var bool: y) =
    member_bool(x, y);

/** @group globals
  Requires that \a y occurs in the array \a x.
*/
predicate member(array[int] of var float: x, var float: y) =
    member_float(x, y);

/** @group globals
  Requires that \a y occurs in the array \a x.
*/
predicate member(array[int] of var int: x, var int: y) =
    member_int(x, y);

/** @group globals
  Requires that \a y occurs in the array \a x.
*/
predicate member(array[int] of var set of int: x, var set of int: y) =
    member_set(x, y);

/** @group globals
  Requires that \a y occurs in the set \a x.
*/
predicate member(var set of int: x, var int: y) =
    set_member(x, y);
