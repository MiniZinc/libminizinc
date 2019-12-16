include "count_fn.mzn";

predicate fzn_count_leq_par(array[int] of var int: x, int: y, int: c) =
  let { var int: z = count(x,y) } in z >= c;
  % This needs to be written with a let rather than count(x,y) >= c
  % so that the automatic rewriting of the latter doesn't kick in

%-----------------------------------------------------------------------------%
