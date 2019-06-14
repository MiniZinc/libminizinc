include "fzn_pwl.mzn";

/** @group globals
  Constrains \a y(\a x) to be a piecewise-linear interpolation of
  the provided point sequence.
*/
predicate pwl(var float: x, var float: y,
              array[int] of float: xi, array[int] of float: vi) = 
    fzn_pwl(x, y, xi, vi);

%-----------------------------------------------------------------------------%
