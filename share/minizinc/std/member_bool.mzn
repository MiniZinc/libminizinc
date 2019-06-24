include "fzn_member_bool.mzn";
include "fzn_member_bool_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that 'y' occurs in the array or set 'x'.
%-----------------------------------------------------------------------------%

predicate member_bool(array[int] of var bool: x, var bool: y) =
    fzn_member_bool(x, y);
