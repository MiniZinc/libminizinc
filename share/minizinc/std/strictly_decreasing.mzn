%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in strict decreasing order
%-----------------------------------------------------------------------------%

predicate strictly_decreasing(array[int] of var bool: x) =
    analyse_alldifferent(x) /\
    fzn_strictly_decreasing_bool(x);

predicate strictly_decreasing(array[int] of var int: x) =
    analyse_alldifferent(x) /\
    fzn_strictly_decreasing_int(x);

include "fzn_strictly_decreasing.mzn";
include "analyse_alldifferent.mzn";
