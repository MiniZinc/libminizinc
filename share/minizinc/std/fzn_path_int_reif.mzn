include "tree.mzn";
include "subgraph.mzn";  

predicate fzn_path_reif(int: N, int: E, array[int] of int: from, array[int] of int: to,
                        var int: s, var int: t, array[int] of var bool: ns, array[int] of var bool: es,
                        var bool: b) =
  abort("Reified path constraint is not supported");
   
%-----------------------------------------------------------------------------%
