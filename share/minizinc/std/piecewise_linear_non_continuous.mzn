include "fzn_piecewise_linear_non_continuous.mzn";
include "fzn_piecewise_linear_non_continuous_reif.mzn";

/** @group globals
  Return the piecewise-linear function of x
  on the given (possibly disconnected) intervals.
  Each interval \a i connects
  (\a x_start[\a i], \a v_start[\a i]) to (\a x_end[\a i], \a v_end[\a i]).
*/
function var float: piecewise_linear(var float: x,
    array[int] of float: x_start, array[int] of float: x_end,
    array[int] of float: v_start, array[int] of float: v_end)  ::promise_total =
  let {
    var float: res,
    constraint piecewise_linear(x, res, x_start,x_end, v_start,v_end),
  } in res;


/** @group globals
  Constrains \a y(\a x) to be the piecewise-linear function on
  the provided point-value intervals.
*/
predicate piecewise_linear(var float: x, var float: y,
              array[int] of float: x_start, array[int] of float: x_end,
              array[int] of float: v_start, array[int] of float: v_end
              ) = 
    fzn_piecewise_linear(x, y, x_start,x_end, v_start,v_end);

%-----------------------------------------------------------------------------%
