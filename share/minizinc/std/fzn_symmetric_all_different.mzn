include "all_different.mzn";
include "inverse.mzn";

predicate fzn_symmetric_all_different(array[int] of var int:x) =
    all_different(x) /\ inverse(x,x);
