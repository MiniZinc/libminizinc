include "fzn_member_int.mzn";
include "fzn_member_int_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that 'y' occurs in the array or set 'x'.
%-----------------------------------------------------------------------------%

predicate member_int(array[int] of var int: x, var int: y) =
    fzn_member_int(x, y);
