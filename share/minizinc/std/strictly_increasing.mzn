include "fzn_strictly_increasing_int.mzn";
include "fzn_strictly_increasing_int_opt.mzn";
include "fzn_strictly_increasing_int_reif.mzn";
include "fzn_strictly_increasing_int_opt_reif.mzn";
include "fzn_strictly_increasing_float.mzn";
include "fzn_strictly_increasing_float_opt.mzn";
include "fzn_strictly_increasing_float_reif.mzn";
include "fzn_strictly_increasing_float_opt_reif.mzn";
include "fzn_strictly_increasing_set.mzn";
include "fzn_strictly_increasing_set_reif.mzn";
include "analyse_all_different.mzn";

predicate fzn_strictly_increasing_float(array [int] of var float: xs);
predicate fzn_strictly_increasing_float_opt(array [int] of var opt float: xs);
predicate fzn_strictly_increasing_int(array [int] of var int: xs);
predicate fzn_strictly_increasing_int_opt(array [int] of var opt int: xs);
predicate fzn_strictly_increasing_set(array [int] of var set of int: xs);

%-----------------------------------------------------------------------------%
% Requires that the array 'xs' is in strict increasing order
%-----------------------------------------------------------------------------%

predicate strictly_increasing(array [$X] of var bool: xs) =
  let {
    array [int] of var bool: xx = array1d(xs);
  } in analyse_all_different(xx) /\
    if length(xx) >= 3 then false elseif length(xx) = 2 then xx = [false, true] else true endif;

/** @group globals.sort Requires that the array \a xs is in a strictly increasing order (duplicates are *not* allowed). */
predicate strictly_increasing(array [$X] of var $$E: xs) =
  analyse_all_different(array1d(xs)) /\ fzn_strictly_increasing_int(enum2int(array1d(xs)));

/** @group globals.sort Requires that the array \a xs is in a strictly increasing order (duplicates are *not* allowed). */
predicate strictly_increasing(array [$X] of var opt $$E: xs) =
  analyse_all_different(array1d(xs)) /\ fzn_strictly_increasing_int_opt(enum2int(array1d(xs)));

/** @group globals.sort Requires that the array \a xs is in increasing order (duplicates are allowed). */
predicate strictly_increasing(array [$X] of var float: xs) =
  fzn_strictly_increasing_float(array1d(xs));

/** @group globals.sort Requires that the array \a xs is in increasing order (duplicates are allowed). */
predicate strictly_increasing(array [$X] of var opt float: xs) =
  fzn_strictly_increasing_float_opt(array1d(xs));

/** @group globals.sort Requires that the array \a xs is in increasing order (duplicates are allowed). */
predicate strictly_increasing(array [$X] of var set of $$E: xs) =
  fzn_strictly_increasing_set(enum2int(array1d(xs)));
