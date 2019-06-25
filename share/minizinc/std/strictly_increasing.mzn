include "fzn_strictly_increasing_int.mzn";
include "fzn_strictly_increasing_int_reif.mzn";
include "fzn_strictly_increasing_bool.mzn";
include "fzn_strictly_increasing_bool_reif.mzn";
include "analyse_all_different.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in strict increasing order
%-----------------------------------------------------------------------------%

predicate strictly_increasing(array[int] of var bool: x) =
    analyse_all_different(x) /\
    fzn_strictly_increasing_bool(x);

predicate strictly_increasing_reif(array[int] of var bool: x, var bool: b) =
    fzn_strictly_increasing_bool_reif(x,b);

predicate strictly_increasing(array[int] of var int: x) =
    analyse_all_different(x) /\
    fzn_strictly_increasing_int(x);

predicate strictly_increasing_reif(array[int] of var int: x, var bool: b) =
    fzn_strictly_increasing_int_reif(x,b);

