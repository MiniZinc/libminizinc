include "fzn_int_set_channel.mzn";

/** @group globals.channeling
  Requires that array of int variables \a x and array of set variables \a y
  are related such that (\a x[\p i] = \p j) &#8596; (\p i in \a y[\p j]).
*/
predicate int_set_channel(array[int] of var int: x,
                          array[int] of var set of int: y) =
    fzn_int_set_channel(x,y);

%-----------------------------------------------------------------------------%
