include "count_fn.mzn";

predicate fzn_count_leq(array[int] of var int: x, var int: y, var int: c) =
  let { var int: z = count(x,y) } in z >= c;
  % This needs to be written with a let rather than count(x,y) >= c
  % so that the automatic rewriting of the latter doesn't kick in

%-----------------------------------------------------------------------------%
