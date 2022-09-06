include "inverse.mzn";

/** @group globals.channeling
  Given a function \a f represented as an array, return the inverse function.
*/
function array[$$E] of var $$F: inverse(array[$$F] of var $$E: f) =
    let {
      array[lb_array(f)..ub_array(f)] of var index_set(f): invf ::is_defined_var;
      constraint inverse(f,invf) ::defines_var(invf);
    } in invf;

/** @group globals.channeling
  Given a function \a f represented as an array, return the inverse function.
*/
function array[$$E] of var opt $$F: inverse(array[$$F] of var opt $$E: f) =
    let {
      array[lb_array(f)..ub_array(f)] of var opt index_set(f): invf ::is_defined_var;
      constraint inverse(f, invf) ::defines_var(invf);
    } in invf;

/** @group globals.channeling
  Given a function \a f represented as an array, return the inverse function.
*/
function array[$$E] of $$F: inverse(array[$$F] of $$E: f);
