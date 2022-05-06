include "fzn_member_bool.mzn";
include "fzn_member_bool_reif.mzn";
include "fzn_member_float.mzn";
include "fzn_member_float_reif.mzn";
include "fzn_member_int.mzn";
include "fzn_member_int_reif.mzn";
include "fzn_member_set.mzn";
include "fzn_member_set_reif.mzn";
include "fzn_set_member.mzn";
include "fzn_set_member_reif.mzn";

/** @group globals.array
  Requires that \a y occurs in the array \a x.
*/
predicate member(array[int] of var bool: x, var bool: y) =
	fzn_member_bool(x, y);

/** @group globals.array
  Requires that \a y occurs in the array \a x.
*/
predicate member(array[int] of var float: x, var float: y) =
	fzn_member_float(x, y);

/** @group globals.array
  Requires that \a y occurs in the array \a x.
*/
predicate member(array[int] of var $$E: x, var $$E: y) =
	fzn_member_int(x, y);

/** @group globals.array
  Requires that \a y occurs in the array \a x.
*/
predicate member(array[int] of var set of $$E: x, var set of $$E: y) =
	fzn_member_set(x, y);

/** @group globals.array
  Requires that \a y occurs in the set \a x.
*/
predicate member(var set of $$E: x, var $$E: y) =
	fzn_set_member(x, y);
