include "fzn_set_member.mzn";
include "fzn_set_member_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that 'y' occurs in the array or set 'x'.
%-----------------------------------------------------------------------------%

predicate set_member(var set of int: x, var int: y) =
    fzn_set_member(x, y);
