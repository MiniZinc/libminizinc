include "all_different.mzn";
include "inverse.mzn";

predicate fzn_symmetric_all_different(array [int] of var int: xs) =
   all_different(xs) /\ inverse(xs, xs);
