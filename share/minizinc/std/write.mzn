include "fzn_write.mzn";
include "fzn_write_reif.mzn";

/** @group globals.array
    Creates a new array \a O from an input array \a I with a change at position \a i to take value \a v

    \a I is an array of values
    \a O is an array of values with same index set as \a I
    \a i is an index for \a I
    \a v is the value to write
*/
predicate write(array [$$E] of var $$T: I, var $$E: i, var $$T: v, array [$$E] of var $$T: O) =
  assert(index_sets_agree(O, I), "writet: index set of I must be same as O") /\
    fzn_write(index2int(enum2int(I)), enum2int(i), enum2int(v), index2int(enum2int(O)));

/** @group globals.array
    Return the input array \a I with a change at position \a i to take value \a v

    \a I is an array of values
    \a i is an index for \a I
    \a v is the value to write 
*/
function array [$$E] of var $$T: write(array [$$E] of var $$T: I, var $$E: i, var $$T: v) =
  let {
    array [index_set(I)] of var dom_array(I) union dom(v): O;
    constraint write(I, i, v, O);
  } in O;

%-----------------------------------------------------------------------------%
