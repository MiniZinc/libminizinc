include "fzn_piecewise_linear_non_continuous.mzn";
include "fzn_piecewise_linear_non_continuous_reif.mzn";

/** @group globals.math
  Return the piecewise-linear function of \a x
  on the given (possibly disconnected) intervals.
  Each interval \p i connects
  (\a x_start[\p i], \a v_start[\p i]) to (\a x_end[\p i], \a v_end[\p i]).
*/
function var float: piecewise_linear(var float: x,
    array[int] of float: x_start, array[int] of float: x_end,
    array[int] of float: v_start, array[int] of float: v_end)  ::promise_total =
  let {
    var float: res,
    constraint piecewise_linear(x, res, x_start,x_end, v_start,v_end),
  } in res;


/** @group globals.math
  Constrains \a y(\a x) to be the piecewise-linear function
  on the given (possibly disconnected) intervals.
  Each interval \p i connects
  (\a x_start[\p i], \a v_start[\p i]) to (\a x_end[\p i], \a v_end[\p i]).
*/
predicate piecewise_linear(var float: x, var float: y,
              array[int] of float: x_start, array[int] of float: x_end,
              array[int] of float: v_start, array[int] of float: v_end
              ) = 
    fzn_piecewise_linear_non_continuous(x, y, x_start,x_end, v_start,v_end);

%-----------------------------------------------------------------------------%
