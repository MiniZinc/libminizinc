include "inverse.mzn";

/** @group globals.channeling
  Given a function \a f represented as an array, return the inverse function.
*/
function array[int] of var int: inverse(array[int] of var int: f) =
    let {
      array[lb_array(f)..ub_array(f)] of var index_set(f): invf;
      constraint inverse(f,invf);
    } in invf;
