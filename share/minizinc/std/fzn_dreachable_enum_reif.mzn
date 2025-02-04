predicate fzn_dreachable_reif(
  array [int] of int: from,
  array [int] of int: to,
  var int: r,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var bool: b,
) = abort("Reified dreachable constraint is not supported");

%-----------------------------------------------------------------------------%
