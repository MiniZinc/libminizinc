predicate fzn_value_precede_set_reif(
  int: before,
  int: after,
  array [int] of var set of int: xs,
  var bool: b,
) = abort("Reified value_precede/3 for sets is not supported.");
