include "count_fn.mzn";

predicate fzn_count_lt_bool(array [int] of var bool: xs, var bool: value, var int: n) =
  let { var int: z = count(xs, value) } in z > n;
% This needs to be written with a let rather than count(xs,value) > n
% so that the automatic rewriting of the latter doesn't kick in

%-----------------------------------------------------------------------------%
