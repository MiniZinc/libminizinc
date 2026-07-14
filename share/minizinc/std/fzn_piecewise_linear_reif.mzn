predicate fzn_piecewise_linear_reif(
  var float: x,
  var float: y,
  array [int] of float: x_knots,
  array [int] of float: y_knots,
  var bool: b,
) = abort("Reified piecewise_linear constraint is not supported");

%-----------------------------------------------------------------------------%
