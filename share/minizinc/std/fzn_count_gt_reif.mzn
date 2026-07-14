include "count_fn.mzn";

predicate fzn_count_gt_reif(array [int] of var int: xs, var int: value, var int: n, var bool: b) =
  let { var int: z = count(xs, value) } in b <-> z < n;
% This needs to be written with a let rather than count(x,y) < c
% so that the automatic rewriting of the latter doesn't kick in

%-----------------------------------------------------------------------------%
