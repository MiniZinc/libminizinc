include "fzn_piecewise_linear.mzn";
include "fzn_piecewise_linear_reif.mzn";

predicate fzn_piecewise_linear(
  var float: x,
  var float: y,
  array [int] of float: x_knots,
  array [int] of float: y_knots,
);

/** @group globals.math
  Return the piecewise-linear function of \a x
  on the given point-value sequence.
  The array \a x_knots defines the points, and \a y_knots the corresponding values.
*/
function var float: piecewise_linear(
  var float: x,
  array [int] of float: x_knots,
  array [int] of float: y_knots,
)
  :: promise_total =
  let {
    var float: res;
    constraint piecewise_linear(x, res, x_knots, y_knots);
  } in res;

/** @group globals.math
  Constrains \a y(\a x) to be the piecewise-linear function
  on the provided point-value sequence.
  The array \a x_knots defines the points, and \a y_knots the corresponding values.
*/
predicate piecewise_linear(
  var float: x,
  var float: y,
  array [int] of float: x_knots,
  array [int] of float: y_knots,
) = fzn_piecewise_linear(x, y, x_knots, y_knots);

%-----------------------------------------------------------------------------%
