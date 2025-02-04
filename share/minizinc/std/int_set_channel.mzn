include "fzn_int_set_channel.mzn";
include "fzn_int_set_channel_reif.mzn";

/** @group globals.channeling
  Requires that array of int variables \a x and array of set variables \a y
  are related such that (\a x[\p i] = \p j) \( \leftrightarrow \) (\p i in \a y[\p j]).
*/
predicate int_set_channel(array [$$I] of var $$J: x, array [$$J] of var set of $$I: y) =
  fzn_int_set_channel(index2int(enum2int(x)), index2int(enum2int(y)));

%-----------------------------------------------------------------------------%
