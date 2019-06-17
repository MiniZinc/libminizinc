include "fzn_piecewise_linear.mzn";

/** @group globals
  Return a piecewise-linear interpolation
  of the given point sequence as a function of \a x
*/
function var float: piecewise_linear(var float: x, array[int] of float: xi, array[int] of float: vi) =
  let {
    var float: res,
    constraint piecewise_linear(x,res,xi,vi),
  } in res;


/** @group globals
  Constrains \a y(\a x) to be a piecewise-linear interpolation of
  the provided point sequence.
*/
predicate piecewise_linear(var float: x, var float: y,
              array[int] of float: xi, array[int] of float: vi) = 
    fzn_piecewise_linear(x, y, xi, vi);

%-----------------------------------------------------------------------------%
