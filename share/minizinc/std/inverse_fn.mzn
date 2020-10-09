include "inverse.mzn";

/** @group globals.channeling
  Given a function \a f represented as an array, return the inverse function.
*/
function array[$$E] of var $$F: inverse(array[$$F] of var $$E: f) =
    let {
      array[lb_array(f)..ub_array(f)] of var index_set(f): invf;
      constraint inverse(f,invf);
    } in invf;

/** @group globals.channeling
  Given a function \a f represented as an array, return the inverse function.
*/
function array[$$E] of $$F: inverse(array[$$F] of $$E: f);
