include "roots.mzn";

/** @group globals.set
  Returns the set of indices \p i such that \a xs[\p i] is in \a image.
*/
function var set of $$X: roots(array [$$X] of var $$Y: xs, var set of $$Y: image) :: promise_total =
  let {
    var set of index_set(xs): preimage :: is_defined_var;
    constraint roots(xs, preimage, image) :: defines_var(preimage);
  } in preimage;
