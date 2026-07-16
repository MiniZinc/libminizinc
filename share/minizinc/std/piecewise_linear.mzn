include "fzn_piecewise_linear.mzn";
include "fzn_piecewise_linear_reif.mzn";

predicate fzn_piecewise_linear(
  var float: x,
  var float: y,
  array [int] of float: x_knot,
  array [int] of float: y_knot,
);

/** @group globals.math
  Return the piecewise-linear function of \a x
  on the given point-value sequence.
  The array \a x_knot defines the points, and \a y_knot the corresponding values.
*/
function var float: piecewise_linear(
  var float: x,
  array [int] of float: x_knot,
  array [int] of float: y_knot,
)
  :: promise_total =
  let {
    var float: res;
    constraint piecewise_linear(x, res, x_knot, y_knot);
  } in res;

/** @group globals.math
  Constrains \a y(\a x) to be the piecewise-linear function
  on the provided point-value sequence.
  The array \a x_knot defines the points, and \a y_knot the corresponding values.
*/
predicate piecewise_linear(
  var float: x,
  var float: y,
  array [int] of float: x_knot,
  array [int] of float: y_knot,
) = fzn_piecewise_linear(x, y, x_knot, y_knot);

%-----------------------------------------------------------------------------%
