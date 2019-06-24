include "fzn_member_set.mzn";
include "fzn_member_set_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that 'y' occurs in the array or set 'x'.
%-----------------------------------------------------------------------------%

predicate member_set(array[int] of var set of int: x, var set of int: y) =
    fzn_member_set(x, y);
