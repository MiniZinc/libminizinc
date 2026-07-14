include "count_fn.mzn";

predicate fzn_count_gt_bool_reif(
  array [int] of var bool: xs,
  var bool: value,
  var int: n,
  var bool: b,
) = let { var int: z = count(xs, value) } in b <-> z < n;
% This needs to be written with a let rather than count(xs,value) < n
% so that the automatic rewriting of the latter doesn't kick in

%-----------------------------------------------------------------------------%
