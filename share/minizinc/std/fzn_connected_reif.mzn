predicate fzn_connected_reif(
  array [int] of int: from,
  array [int] of int: to,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var bool: b,
) = abort("Reified connected is not supported.");

%-----------------------------------------------------------------------------%
