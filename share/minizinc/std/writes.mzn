include "fzn_writes.mzn";
include "fzn_writes_reif.mzn";

/** @group globals.array
    Creates a new array \a O from an input array \a I with a simultaneous change at positions \a P to values \a V

    \a I is an array of input values
    \a O is an array of integers with same index set as \a I
    \a P is an array of index values in \a I
    \a V is an array of values to write 
*/
predicate writes(
  array [$$X] of var $$T: I,
  array [$$Y] of var $$X: P,
  array [$$Y] of var $$T: V,
  array [$$X] of var $$T: O,
) =
  assert(index_set(O) = index_set(I), "writes: index set of I must be same as O") /\
    assert(index_set(P) = index_set(V), "writes: index set of P must be same as V") /\
    fzn_writes(
      index2int(enum2int(I)),
      index2int(enum2int(P)),
      index2int(enum2int(V)),
      index2int(enum2int(O)),
    );

/** @group globals.array
    Returns a new array from an input array \a I with a simultaneous change at positions \a P to values \a V

    \a I is an array of input values
    \a P is an array of index values in \a I
    \a V is an array of values to write 
*/
function array [$$X] of var $$T: writes(
  array [$$X] of var $$T: I,
  array [$$Y] of var $$X: P,
  array [$$Y] of var $$T: V,
)
  :: promise_total =
  let {
    array [index_set(I)] of var dom_array(I) union dom_array(V): O;
    constraint writes(I, P, V, O);
  } in O;

%-----------------------------------------------------------------------------%
