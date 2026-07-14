predicate fzn_dwst_reif(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  array [int] of int: w,
  var int: root,
  array [int] of var bool: es,
  var int: weight_bound,
  var bool: b,
) = abort("Reified dwst constraint is not supported");

%-----------------------------------------------------------------------------%
