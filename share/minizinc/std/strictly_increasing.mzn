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

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in strict increasing order
%-----------------------------------------------------------------------------%

predicate strictly_increasing(array [$X] of var bool: x) =
  let {
    array [int] of var bool: xx = array1d(x);
  } in analyse_all_different(xx) /\
    if length(xx) >= 3 then false elseif length(xx) = 2 then xx = [false, true] else true endif;

/** @group globals.sort Requires that the array \a x is in a stricly increasing order (duplicates are *not* allowed). */
predicate strictly_increasing(array [$X] of var $$E: x) =
  analyse_all_different(array1d(x)) /\ fzn_strictly_increasing_int(enum2int(array1d(x)));

/** @group globals.sort Requires that the array \a x is in a stricly increasing order (duplicates are *not* allowed). */
predicate strictly_increasing(array [$X] of var opt $$E: x) =
  analyse_all_different(array1d(x)) /\ fzn_strictly_increasing_int_opt(enum2int(array1d(x)));

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate strictly_increasing(array [$X] of var float: x) =
  fzn_strictly_increasing_float(array1d(x));

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate strictly_increasing(array [$X] of var opt float: x) =
  fzn_strictly_increasing_float_opt(array1d(x));

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate strictly_increasing(array [$X] of var set of $$E: x) =
  fzn_strictly_increasing_set(enum2int(array1d(x)));
