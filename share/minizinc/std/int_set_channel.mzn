include "fzn_int_set_channel.mzn";
include "fzn_int_set_channel_reif.mzn";

predicate fzn_int_set_channel(array [int] of var int: ints, array [int] of var set of int: sets);

/** @group globals.channeling
  Requires that array of int variables \a ints and array of set variables \a sets
  are related such that (\a ints[\p i] = \p j) \( \leftrightarrow \) (\p i in \a sets[\p j]).
*/
predicate int_set_channel(array [$$I] of var $$J: ints, array [$$J] of var set of $$I: sets) =
  fzn_int_set_channel(index2int(enum2int(ints)), index2int(enum2int(sets)));

%-----------------------------------------------------------------------------%
