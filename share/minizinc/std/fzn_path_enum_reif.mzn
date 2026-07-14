include "tree.mzn";
include "subgraph.mzn";

predicate fzn_path_reif(
  array [int] of int: from,
  array [int] of int: to,
  var int: source,
  var int: sink,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var bool: b,
) = abort("Reified path constraint is not supported");

%-----------------------------------------------------------------------------%
