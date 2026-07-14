predicate fzn_piecewise_linear_non_continuous_reif(
  var float: x,
  var float: y,
  array [int] of float: x_start,
  array [int] of float: x_end,
  array [int] of float: y_start,
  array [int] of float: y_end,
  var bool: b,
) = abort("Reified piecewise_linear (non_continuous) constraint is not supported");

%-----------------------------------------------------------------------------%
