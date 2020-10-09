include "count_fn.mzn";

predicate fzn_count_gt_reif(array[int] of var int: x, var int: y, var int: c, var bool: b) =
  let { var int: z = count(x,y) } in b <-> z < c;
  % This needs to be written with a let rather than count(x,y) < c
  % so that the automatic rewriting of the latter doesn't kick in

%-----------------------------------------------------------------------------%
