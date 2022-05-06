include "fzn_piecewise_linear.mzn";
include "fzn_piecewise_linear_reif.mzn";

/** @group globals.math
  Return the piecewise-linear function of \a x
  on the given point-value sequence.
  The array \a xi defines the points, and \a vi the corresponding values.
*/
function var float: piecewise_linear(var float: x, array[int] of float: xi, array[int] of float: vi) ::promise_total =
  let {
    var float: res,
    constraint piecewise_linear(x,res,xi,vi),
  } in res;


/** @group globals.math
  Constrains \a y(\a x) to be the piecewise-linear function
  on the provided point-value sequence.
  The array \a xi defines the points, and \a vi the corresponding values.
*/
predicate piecewise_linear(var float: x, var float: y,
              array[int] of float: xi, array[int] of float: vi) = 
    fzn_piecewise_linear(x, y, xi, vi);

%-----------------------------------------------------------------------------%
