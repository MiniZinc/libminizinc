include "fzn_all_different_int.mzn";
include "fzn_all_different_int_opt.mzn";
include "fzn_all_different_int_reif.mzn";
include "fzn_all_different_set.mzn";
include "fzn_all_different_set_reif.mzn";
include "fzn_alldifferent_except.mzn";
include "fzn_alldifferent_except_reif.mzn";
include "analyse_all_different.mzn";

predicate fzn_all_different_int(array [int] of var int: xs);
predicate fzn_all_different_int_opt(array [int] of var opt int: xs);
predicate fzn_all_different_set(array [int] of var set of int: xs);
predicate fzn_alldifferent_except(array [int] of var int: xs, set of int: except);

/** @group globals.alldifferent
  Constrain the elements in the array \a xs to be pairwise different, except for
  those that take a value in the set \a except (values in \a except may occur
  any number of times).
*/
predicate all_different(array [$X] of var $$E: xs, set of $$E: except = {}) =
  if except = {} then
    analyse_all_different(array1d(xs)) /\ fzn_all_different_int(enum2int(array1d(xs)))
  else
    fzn_alldifferent_except(enum2int(array1d(xs)), enum2int(except))
  endif;

/**  @group globals.alldifferent
  Constrain the elements in the array \a xs to be pairwise different.
*/
predicate all_different(array [$X] of var set of $$E: xs) =
  fzn_all_different_set(enum2int(array1d(xs)));

/**  @group globals.alldifferent
  Constrain the non-absent elements in the array \a xs to be pairwise different.
  The absent value <> is allowed to occur multiple times.
*/
predicate all_different(array [$X] of var opt $$E: xs) =
  analyse_all_different(array1d(xs)) /\ fzn_all_different_int_opt(enum2int(array1d(xs)));

% Synonyms for the above.
predicate alldifferent(array [$X] of var $$E: xs, set of $$E: except = {}) =
  all_different(array1d(xs), except);

predicate alldifferent(array [$X] of var set of $$E: xs) = all_different(array1d(xs));

predicate alldifferent(array [$X] of var opt $$E: xs) = all_different(array1d(xs));
