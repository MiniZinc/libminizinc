include "fzn_writes_seq.mzn";
include "fzn_writes_seq_reif.mzn";

/** @group globals.array
    Creates a new array \a O from an input array \a I with a sequence of changes at positions \a P to take values \a V

    \a I is an array of input values
    \a O is an array of output values with same index set as \a I
    \a P is an array of indices in \a I
    \a V is an array of values to write 
*/
predicate writes_seq(
  array [$$X] of var $$T: I,
  array [$$Y] of var $$X: P,
  array [$$Y] of var $$T: V,
  array [$$X] of var $$T: O,
) =
  assert(index_sets_agree(O, I), "writes: index set of I must be same as O") /\
    assert(index_sets_agree(P, V), "writes: index set of P must be same as V") /\
    fzn_writes_seq(
      index2int(enum2int(I)),
      index2int(enum2int(P)),
      index2int(enum2int(V)),
      index2int(enum2int(O)),
    );

/** @group globals.array
    Return the input array \a I with a sequence of changes at positions \a P to take values \a V

    \a I is an array of input values
    \a P is an array of index values in \a I
    \a V is an array of values to write
*/
function array [$$X] of var $$T: writes_seq(
  array [$$X] of var $$T: I,
  array [$$Y] of var $$X: P,
  array [$$Y] of var $$T: V,
) =
  let {
    array [index_set(I)] of var dom_array(I) union dom_array(V): O;
    constraint writes_seq(I, P, V, O);
  } in O;

%-----------------------------------------------------------------------------%
